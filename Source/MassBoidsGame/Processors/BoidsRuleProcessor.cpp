// Copyright Dennis Andersson. All Rights Reserved.


#include "BoidsRuleProcessor.h"
#include "Fragments/BoidsLocationFragment.h"
#include "BoidsTypes.h"

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "Async/ParallelFor.h"


UBoidsRuleProcessor::UBoidsRuleProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ExecutionOrder.ExecuteInGroup = MassBoidsGame::ProcessorGroupNames::Boids;
}

void UBoidsRuleProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	
	BoidsSettings = GetMutableDefault<UBoidsSettings>();
	check(BoidsSettings);

	const int32 NumGridsSqrt = BoidsSettings->Extent / BoidsSettings->GridSize;
	const int32 NumGrids = NumGridsSqrt * NumGridsSqrt;

	BoidsPerGrid.AddDefaulted(NumGrids);
	BoidsGrid.AddDefaulted(NumGrids);
}

void UBoidsRuleProcessor::ConfigureQueries()
{
	Entities
		.AddRequirement<FBoidsLocationFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All)
		.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All);
}

void UBoidsRuleProcessor::Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsRuleProcessor);

	TArray<const FVector*> AllLocations;
	TArray<FVector*> AllVelocities;

	// Get locations and velocities for all entities
	Entities.ForEachEntityChunk(EntitySubsystem, Context, [&AllLocations, &AllVelocities] (FMassExecutionContext& Context)
	{
		const TConstArrayView<FBoidsLocationFragment>& Locations = Context.GetFragmentView<FBoidsLocationFragment>();
		const TArrayView<FMassVelocityFragment>& Velocities = Context.GetMutableFragmentView<FMassVelocityFragment>();

		for (int32 Ndx = 0; Ndx < Context.GetNumEntities(); Ndx++)
		{
			AllLocations.Add(&Locations[Ndx].Location);
			AllVelocities.Add(&Velocities[Ndx].Value);
		}
	});

	// TODO: Make sure no other thread is writing to FMassVelocityFragment at the same time
	// Ideally we should be able to pass in a boolean to ForEachEntityChunk to specify if we 
	// want to ClearExecutionData so that we can do it manually. 

	const int32 NumBoids = AllVelocities.Num();

	// Calculates the grid of each boid
	SetupBoidsGrid(AllLocations, NumBoids);

	// Get all the rules for each boid
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidRules);
		
		RunBoidsAlignment(AllLocations, NumBoids);
		RunBoidsSeparation(AllLocations, NumBoids);
		RunBoidsCohesion(AllLocations, AllVelocities, NumBoids);
	}

	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ApplyBoidRules);

		// Apply all of the rules to the boids
		ParallelFor(NumBoids, [this, &AllVelocities] (int32 Ndx)
		{
			(*AllVelocities[Ndx]) += BoidAlignments[Ndx] + BoidSeparations[Ndx] + BoidCohesions[Ndx];
		});
	}
}

void UBoidsRuleProcessor::SetupBoidsGrid(TArray<const FVector*>& BoidLocations, const int32 NumBoids)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SetupBoidsGrid);

	BoidsGridIndex.Init(INDEX_NONE, NumBoids);
	
	// Get the grid for each boid
	ParallelFor(NumBoids, [this, &BoidLocations] (int32 Ndx)
	{
		const int32 NumGridsSqrt = BoidsSettings->Extent / BoidsSettings->GridSize;

		const float GridOriginX = ((BoidsSettings->GridSize * NumGridsSqrt) / 2) - BoidsSettings->Origin.X;
		const float GridOriginY = ((BoidsSettings->GridSize * NumGridsSqrt) / 2) - BoidsSettings->Origin.Y;
		
		const int32 GridX = (int32)((*BoidLocations[Ndx]).X + GridOriginX) / BoidsSettings->GridSize;
		const int32 GridY = (int32)((*BoidLocations[Ndx]).Y + GridOriginY) / BoidsSettings->GridSize;
		
		const bool bValidX = GridX >= 0 && GridX < NumGridsSqrt;
		const bool bValidY = GridY >= 0 && GridY < NumGridsSqrt;
		
		if (bValidX && bValidY)
		{
			BoidsGridIndex[Ndx] = GridY * NumGridsSqrt + GridX;
		}
	});

	for (int32 Ndx = 0; Ndx < BoidsGrid.Num(); Ndx++)
	{
		BoidsGrid[Ndx].Empty(BoidsPerGrid[Ndx]);
	}
	
	for (int32 Ndx = 0; Ndx < NumBoids; Ndx++)
	{
		const uint32 BoidGridIndex = BoidsGridIndex[Ndx];
		if (BoidGridIndex != INDEX_NONE)
		{
			BoidsGrid[BoidGridIndex].Add(Ndx);
		}
	}
}

void UBoidsRuleProcessor::RunBoidsAlignment(TArray<const FVector*>& BoidLocations, const int32 NumBoids)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_RunBoidsAlignment);

	BoidAlignments.Init(FVector(), NumBoids);
	
	const float Alignment = FMath::Clamp(BoidsSettings->AlignmentDistanceSquared, 0.f, 1.0f) / 100.f;
	
	ParallelFor(NumBoids, [this, &BoidLocations, Alignment] (int32 Ndx)
	{
		const int32 BoidGridNdx = BoidsGridIndex[Ndx];
		if (BoidGridNdx != INDEX_NONE)
		{
			const TArray<int32>& NearbyBoids = BoidsGrid[BoidGridNdx];

			if (const int32 NumNearbyBoids = NearbyBoids.Num())
			{
				// Local variable to avoid cache misses when doing iteration below
				FVector BoidAlignment = BoidAlignments[Ndx];
				const FVector BoidLocation = (*BoidLocations[Ndx]);

				uint32 NumInRange = 0;
				
				for (int32 OtherNdx = 0; OtherNdx < NumNearbyBoids; OtherNdx++)
				{
					const int32 OtherBoidNdx = NearbyBoids[OtherNdx];
					const FVector OtherBoidLocation = *BoidLocations[OtherBoidNdx];
					
					if (FVector::DistSquared(BoidLocation, OtherBoidLocation) < BoidsSettings->AlignmentDistanceSquared)
					{
						BoidAlignment += OtherBoidLocation;
						++NumInRange;
					}
				}

				if (NumInRange)
				{
					BoidAlignment /= NumInRange;
					BoidAlignment = (BoidAlignment - BoidLocation) * Alignment;

					BoidAlignments[Ndx] = BoidAlignment;
				}
			}
		}
	});
}

void UBoidsRuleProcessor::RunBoidsSeparation(TArray<const FVector*>& BoidLocations, const int32 NumBoids)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_RunBoidsSeparation);

	BoidSeparations.Init(FVector(), NumBoids);
	
	const float Separation = FMath::Clamp(BoidsSettings->Separation, 0.f, 1.0f) / 10.f;

	ParallelFor(NumBoids, [this, &BoidLocations, Separation] (int32 Ndx)
	{
		const int32 BoidGridNdx = BoidsGridIndex[Ndx];
		if (BoidGridNdx != INDEX_NONE)
		{
			const TArray<int32>& NearbyBoids = BoidsGrid[BoidGridNdx];

			if (NearbyBoids.Num())
			{
				// Local variable to avoid cache misses when doing iteration below
				FVector BoidSeparation = BoidSeparations[Ndx];
				const FVector BoidLocation = *BoidLocations[Ndx];
			
				for (int32 OtherNdx = 0; OtherNdx < NearbyBoids.Num(); OtherNdx++)
				{
					const int32 OtherBoidNdx = NearbyBoids[OtherNdx];
					const FVector OtherBoidLocation = *BoidLocations[OtherBoidNdx];
				
					if (OtherBoidNdx != Ndx && FVector::DistSquared(BoidLocation, OtherBoidLocation) < BoidsSettings->SeparationDistanceSquared)
					{
						BoidSeparation += BoidLocation - OtherBoidLocation;
					}
				}

				BoidSeparations[Ndx] = BoidSeparation * Separation;
			}
		}
	});
}

void UBoidsRuleProcessor::RunBoidsCohesion(TArray<const FVector*>& BoidLocations, TArray<FVector*>& BoidVelocities, const int32 NumBoids)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_RunBoidsCohesion);

	BoidCohesions.Init(FVector(), NumBoids);

	const float Cohesion = FMath::Clamp(BoidsSettings->Cohesion, 0.f, 1.0f) / 10.f;

	ParallelFor(NumBoids, [this, &BoidLocations, &BoidVelocities, Cohesion] (int32 Ndx)
	{
		const int32 BoidGridNdx = BoidsGridIndex[Ndx];
		if (BoidGridNdx != INDEX_NONE)
		{
			const TArray<int32>& NearbyBoids = BoidsGrid[BoidGridNdx];

			if (const int32 NumNearbyBoids = NearbyBoids.Num())
			{
				// Local variable to avoid cache misses when doing iteration below
				FVector BoidCohesion = BoidCohesions[Ndx];
				const FVector BoidLocation = *BoidLocations[Ndx];
				const FVector BoidVelocity = *BoidVelocities[Ndx];

				uint32 NumInRange = 0;
			
				for (int32 OtherNdx = 0; OtherNdx < NumNearbyBoids; OtherNdx++)
				{
					const int32 OtherBoidNdx = NearbyBoids[OtherNdx];
					if (OtherBoidNdx != Ndx && FVector::DistSquared(BoidLocation, (*BoidLocations[OtherBoidNdx])) < BoidsSettings->CohesionDistanceSquared)
					{
						BoidCohesion += *BoidVelocities[OtherBoidNdx];
						++NumInRange;
					}
				}

				if (NumInRange)
				{
					BoidCohesion /= NumInRange;
					BoidCohesion = (BoidCohesion - BoidVelocity) * Cohesion;

					BoidCohesions[Ndx] = BoidCohesion;
				}
			}
		}
	});
}