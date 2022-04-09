// Copyright Dennis Andersson. All Rights Reserved.


#include "BoidsSpawnProcessor.h"
#include "BoidsRuleProcessor.h"
#include "MassMovementFragments.h"
#include "Fragments/BoidsLocationFragment.h"
#include "Fragments/BoidsSpawnTag.h"
#include "Fragments/BoidsSpeedFragment.h"
#include "Subsystems/BoidsSubsystem.h"
#include "BoidsTypes.h"

// Engine
#include "Engine/World.h"
#include "MassSpawnerTypes.h"


UBoidsSpawnProcessor::UBoidsSpawnProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ExecutionOrder.ExecuteInGroup = MassBoidsGame::ProcessorGroupNames::Boids;
	ExecutionOrder.ExecuteBefore.Add(UBoidsRuleProcessor::StaticClass()->GetFName());
	bAutoRegisterWithProcessingPhases = false;
}

void UBoidsSpawnProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	BoidsSubsystem = UWorld::GetSubsystem<UBoidsSubsystem>(Owner.GetWorld());
	check(BoidsSubsystem);
}

void UBoidsSpawnProcessor::ConfigureQueries()
{
	Entities
		.AddRequirement<FBoidsLocationFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All)
		.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All)
		.AddRequirement<FBoidsSpeedFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All)
		.AddTagRequirement<FBoidsSpawnTag>(EMassFragmentPresence::All);
}

void UBoidsSpawnProcessor::Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsSpawnLocationProcessor);

	if (!ensure(Context.ValidateAuxDataType<FMassTransformsSpawnData>()))
	{
		return;
	}
	
	const UWorld* World = EntitySubsystem.GetWorld();
	check(World);
	
	if (World->GetNetMode() != NM_Client)
	{
		FMassTransformsSpawnData& AuxData = Context.GetMutableAuxData().GetMutable<FMassTransformsSpawnData>();
		TArray<FTransform>& Transforms = AuxData.Transforms;

		const int32 NumSpawnTransforms = Transforms.Num();
		if (NumSpawnTransforms)
		{
			int32 NumRequiredSpawnTransforms = 0;
			Entities.ForEachEntityChunk(EntitySubsystem, Context, [&NumRequiredSpawnTransforms](const FMassExecutionContext& Context)
			{
				NumRequiredSpawnTransforms += Context.GetNumEntities();
			});

			const int32 NumToAdd = NumRequiredSpawnTransforms - NumSpawnTransforms;

			// Make sure each entity has a location to spawn in
			if (NumToAdd > 0)
			{
				Transforms.AddUninitialized(NumToAdd);
				for (int32 Ndx = 0; Ndx < NumToAdd; Ndx++)
				{
					Transforms[NumSpawnTransforms + Ndx] = Transforms[FMath::RandRange(0, NumSpawnTransforms - 1)];
				}
			}

			// Set the Location and Velocity of each boids
			Entities.ForEachEntityChunk(EntitySubsystem, Context, [&Transforms, this](FMassExecutionContext& Context)
			{
				const TArrayView<FBoidsLocationFragment> Locations = Context.GetMutableFragmentView<FBoidsLocationFragment>();
				const TArrayView<FMassVelocityFragment> Velocities = Context.GetMutableFragmentView<FMassVelocityFragment>();
				const TConstArrayView<FBoidsSpeedFragment> Speeds = Context.GetFragmentView<FBoidsSpeedFragment>();

				const int32 NumEntities = Context.GetNumEntities();
				for (int32 Ndx = 0; Ndx < NumEntities; Ndx++)
				{
					const int32 AuxIndex = FMath::RandRange(0, Transforms.Num() - 1);
					
					Locations[Ndx].Location = Transforms[AuxIndex].GetLocation();
					Velocities[Ndx].Value = Transforms[AuxIndex].GetRotation().Vector() * Speeds[Ndx].MaxSpeed;
					
					Transforms.RemoveAtSwap(AuxIndex, 1, false);
				}
			});
		}

		//
		// Remove the SpawnedTag for all entities that have a spawn tag at the end of this processing phase
		// This way all Processors will only process boids with the SpawnTag for a single frame
		//
		// All Boids are Created with a FBoidsSpawnTag to tell processors that this boids was created this frame so that
		// they can do initial setup or whatever they need. Then this tag is removed for the next frame to signal that they no longer need this setup
		FMassCommandBuffer& RemoveTagBuffer = BoidsSubsystem->GetEndCommandBuffer(EMassProcessingPhase::PrePhysics);
		Entities.ForEachEntityChunk(EntitySubsystem, Context, [this, &RemoveTagBuffer] (FMassExecutionContext& Context)
		{
			for (const FMassEntityHandle& Entity : Context.GetEntities())
			{
				RemoveTagBuffer.RemoveTag<FBoidsSpawnTag>(Entity);
			}
		});
	}
}
