// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MassProcessingTypes.h"
#include "Actors/BoidsRenderActor.h"
#include "BoidsSubsystem.generated.h"

class UMassActorSpawnerSubsystem;
class UMassSimulationSubsystem;
class UMassEntitySubsystem;

/**
 * Subsystem for Boids world
 */
UCLASS()
class MASSBOIDSGAME_API UBoidsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	/** Command Buffers for each processing phase end */
	TMap<EMassProcessingPhase, TSharedPtr<FMassCommandBuffer>> PhaseEndCommandBuffers;
	
	/** Delegate Handle for the OnProcessingPhaseFinished Event */
	TMap<EMassProcessingPhase, FDelegateHandle> ProcessingPhaseFinishedHandle;
	
	UPROPERTY(Transient)
	UMassSimulationSubsystem* SimulationSubsystem;

	UPROPERTY(Transient)
	UMassEntitySubsystem* EntitySubsystem;

	UPROPERTY(Transient)
	ABoidsRenderActor* RenderActor;

public:
	
	// ~ begin USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~ end USubsystem interface

	// ~ begin UWorldSubsystem interface
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// ~ end UWorldSubsystem interface

	/** Get the command buffer for the processing phase ending */
	FORCEINLINE FMassCommandBuffer& GetEndCommandBuffer(const EMassProcessingPhase InPhase) const
	{
		return *PhaseEndCommandBuffers[InPhase].Get();
	}

	/** Gets the actor responsible for rendering boids */
	FORCEINLINE ABoidsRenderActor* GetRenderActor() const
	{
		return RenderActor;
	}

private:
	
	void OnProcessingPhaseFinished(const float DeltaSeconds, const EMassProcessingPhase Phase);
};
