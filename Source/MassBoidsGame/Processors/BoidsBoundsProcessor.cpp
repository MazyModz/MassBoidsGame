// Copyright Dennis Andersson. All Rights Reserved.


#include "BoidsBoundsProcessor.h"
#include "BoidsTypes.h"
#include "BoidsRenderProcessor.h"
#include "BoidsRuleProcessor.h"
#include "Fragments/BoidsLocationFragment.h"

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"


UBoidsBoundsProcessor::UBoidsBoundsProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ExecutionOrder.ExecuteAfter.Add(UBoidsRuleProcessor::StaticClass()->GetFName());
	ExecutionOrder.ExecuteBefore.Add(UBoidsRenderProcessor::StaticClass()->GetFName());
	ExecutionOrder.ExecuteInGroup = MassBoidsGame::ProcessorGroupNames::Boids;
}

void UBoidsBoundsProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	
	BoidsSettings = GetMutableDefault<UBoidsSettings>();
	check(BoidsSettings);
}

void UBoidsBoundsProcessor::ConfigureQueries()
{
	Entities
		.AddRequirement<FBoidsLocationFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All)
		.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All);
}

void UBoidsBoundsProcessor::Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsBoundsProcessor);
	
	Entities.ParallelForEachEntityChunk(EntitySubsystem, Context, [this] (FMassExecutionContext& Context)
	{
		const FVector MinExtent = FVector(-(BoidsSettings->Extent / 2.f));
		const FVector MaxExtent = FVector((BoidsSettings->Extent / 2.f));

		const FBox BoundingBox = FBox(MinExtent - BoidsSettings->TurnBackOffset, MaxExtent + BoidsSettings->TurnBackOffset);
		
		const TArrayView<FMassVelocityFragment> Velocities = Context.GetMutableFragmentView<FMassVelocityFragment>();
		const TConstArrayView<FBoidsLocationFragment> Locations = Context.GetFragmentView<FBoidsLocationFragment>();

		const int32 NumEntities = Context.GetNumEntities();
		
		for (int32 Ndx = 0; Ndx < NumEntities; Ndx++)
		{
			const float& TurnRate = BoidsSettings->TurnBackRate;
			const FVector& Location = Locations[Ndx].Location;
			FVector& Velocity = Velocities[Ndx].Value;
			
			const bool bMinX = Location.X < BoundingBox.Min.X;
			const bool bMinY = Location.Y < BoundingBox.Min.Y;
			const bool bMinZ = Location.Z < BoundingBox.Min.Z;

			// Turn back if outside minimum bounds
			if (bMinX || bMinY || bMinZ)
			{
				Velocity.X += TurnRate * bMinX;
				Velocity.Y += TurnRate * bMinY;
				Velocity.Z += TurnRate * bMinZ;
			}

			const bool bMaxX = Location.X > BoundingBox.Max.X;
			const bool bMaxY = Location.Y > BoundingBox.Max.Y;
			const bool bMaxZ =Location.Z > BoundingBox.Max.Z;

			// Turn back if outside maximum bounds
			if (bMaxX || bMaxY || bMaxZ)
			{
				Velocity.X -= TurnRate * bMaxX;
				Velocity.Y -= TurnRate * bMaxY;
				Velocity.Z -= TurnRate * bMaxZ;
			}
		}
	});
}
