// Copyright Dennis Andersson. All Rights Reserved.


#include "BoidsRenderProcessor.h"
#include "BoidsMoveProcessor.h"
#include "Fragments/BoidsLocationFragment.h"
#include "Fragments/BoidsMeshFragment.h"
#include "Fragments/BoidsSpawnTag.h"
#include "Actors/BoidsRenderActor.h"
#include "Subsystems/BoidsSubsystem.h"
#include "BoidsTypes.h"

// Engine
#include "MassActorSubsystem.h"
#include "MassMovementFragments.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"

UBoidsRenderProcessor::UBoidsRenderProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bRequiresGameThreadExecution = true;
	ExecutionOrder.ExecuteInGroup = MassBoidsGame::ProcessorGroupNames::Boids;
	ExecutionOrder.ExecuteAfter.Add(UBoidsMoveProcessor::StaticClass()->GetFName());
}

void UBoidsRenderProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	
	BoidsSubsystem = UWorld::GetSubsystem<UBoidsSubsystem>(Owner.GetWorld());
	check(BoidsSubsystem);
}

void UBoidsRenderProcessor::ConfigureQueries()
{
	Entities
		.AddRequirement<FBoidsLocationFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All)
		.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All)
		.AddTagRequirement<FBoidsSpawnTag>(EMassFragmentPresence::Optional)
		.AddConstSharedRequirement<FBoidsMeshFragment>(EMassFragmentPresence::All);
}

void UBoidsRenderProcessor::Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsRenderProcessor);
	
	ABoidsRenderActor* RenderActor = BoidsSubsystem->GetRenderActor();
	if (RenderActor)
	{
		// Create render components for each mesh that boids can have
		Entities.ForEachEntityChunk(EntitySubsystem, Context, [&RenderActor] (FMassExecutionContext& Context)
		{
			const FBoidsMeshFragment* SharedMesh = Context.GetConstSharedFragmentPtr<FBoidsMeshFragment>();
			RenderActor->CreateNewRenderComponent(SharedMesh);
		});

		TMap<const FBoidsMeshFragment*, TArray<FTransform>> BoidXForms;
		TMap<const FBoidsMeshFragment*, TArray<FTransform>> NewBoidXForms;

		// Get the transform for each entity
		Entities.ForEachEntityChunk(EntitySubsystem, Context, [&BoidXForms, &NewBoidXForms] (FMassExecutionContext& Context)
		{
			const int32 NumEntities = Context.GetNumEntities();
			const bool bNewlySpawned = Context.DoesArchetypeHaveTag<FBoidsSpawnTag>();

			const FBoidsMeshFragment* SharedMesh = Context.GetConstSharedFragmentPtr<FBoidsMeshFragment>();

			TArray<FTransform>& XForms = bNewlySpawned ? NewBoidXForms.FindOrAdd(SharedMesh) : BoidXForms.FindOrAdd(SharedMesh);
			XForms.Reserve(XForms.Num() + NumEntities);
		
			const TConstArrayView<FBoidsLocationFragment>& Locations = Context.GetFragmentView<FBoidsLocationFragment>();
			const TConstArrayView<FMassVelocityFragment>& Velocities = Context.GetFragmentView<FMassVelocityFragment>();

			// Get Transform from all Entities
			for (int32 Ndx = 0; Ndx < NumEntities; Ndx++)
			{
				XForms.Add(FTransform
				(
					Velocities[Ndx].Value.Rotation() - FRotator(90.f, 0.f, 0.f),
					Locations[Ndx].Location,
					FVector::OneVector
				));
			}
		});

		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_UpdateRenderComponents);
			
			// Update existing instances
			for (auto&& PairIt : BoidXForms)
			{
				UInstancedStaticMeshComponent* RenderComponent = RenderActor->GetRenderComponent(PairIt.Key);
				check(RenderComponent);

				RenderComponent->BatchUpdateInstancesTransforms
				(
					0,
					PairIt.Value,
					true,
					true,
					true
				);
			}

			// Add new instances
			for (auto&& PairIt : NewBoidXForms)
			{
				UInstancedStaticMeshComponent* RenderComponent = RenderActor->GetRenderComponent(PairIt.Key);
				check(RenderComponent);

				RenderComponent->AddInstances
				(
					PairIt.Value,
					false,
					true
				);
			}
		}
	}
}
