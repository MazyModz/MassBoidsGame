// Copyright Dennis Andersson. All Rights Reserved.


#include "BoidsSubsystem.h"

#include "Engine/World.h"
#include "Subsystems/SubsystemCollection.h"
#include "MassActorSpawnerSubsystem.h"
#include "MassCommandBuffer.h"
#include "MassEntitySubsystem.h"
#include "MassSimulationSubsystem.h"

void UBoidsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	Collection.InitializeDependency<UMassSimulationSubsystem>();
	Collection.InitializeDependency<UMassEntitySubsystem>();

	UWorld* MyWorld = GetWorld();
	check(MyWorld);
	
	SimulationSubsystem = UWorld::GetSubsystem<UMassSimulationSubsystem>(MyWorld);
	check(SimulationSubsystem);
	
	EntitySubsystem = UWorld::GetSubsystem<UMassEntitySubsystem>(MyWorld);
	check(EntitySubsystem);

	// Create Command buffers for each processing phase and bind the flush command
	for (int32 Ndx = 0; Ndx < static_cast<int32>(EMassProcessingPhase::MAX); Ndx++)
	{
		EMassProcessingPhase Phase = static_cast<EMassProcessingPhase>(Ndx);

		PhaseEndCommandBuffers.Add(Phase, MakeShareable(new FMassCommandBuffer));
		
		FDelegateHandle Delegate = SimulationSubsystem->GetOnProcessingPhaseFinished(Phase)
			.AddUObject(this, &UBoidsSubsystem::OnProcessingPhaseFinished, Phase);
		
		ProcessingPhaseFinishedHandle.Add(Phase, Delegate);
	}
}

void UBoidsSubsystem::Deinitialize()
{
	// Unbind the ProcessingPhase event delegates
	for (auto&& PairIt : ProcessingPhaseFinishedHandle)
	{
		SimulationSubsystem->GetOnProcessingPhaseFinished(PairIt.Key).Remove(PairIt.Value);
	}

	// Reset the command buffer shared ptrs
	for (auto&& PairIt : PhaseEndCommandBuffers)
	{
		PhaseEndCommandBuffers[PairIt.Key].Reset();
	}

	// Make sure the Render actor is destroyed along with the subsystem
	UWorld* MyWorld = GetWorld();
	if (RenderActor && MyWorld)
	{
		MyWorld->DestroyActor(RenderActor);
	}
	
	Super::Deinitialize();
}

void UBoidsSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Spawn the render actor in the world
	if (ensureMsgf(!RenderActor, TEXT("RenderActor should not exist on BeginPlay!")))
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.bNoFail = true;
		SpawnInfo.ObjectFlags = RF_Transient;
		
		RenderActor = InWorld.SpawnActor<ABoidsRenderActor>(SpawnInfo);
	}
}

void UBoidsSubsystem::OnProcessingPhaseFinished(const float DeltaSeconds, const EMassProcessingPhase Phase)
{
	// Execute command buffer for the phase
	if (PhaseEndCommandBuffers.Contains(Phase))
	{
		const TSharedPtr<FMassCommandBuffer> BufferPtr = PhaseEndCommandBuffers[Phase];
		if (BufferPtr.IsValid())
		{
			check(EntitySubsystem);
			EntitySubsystem->FlushCommands(BufferPtr);
		}
	}
}
