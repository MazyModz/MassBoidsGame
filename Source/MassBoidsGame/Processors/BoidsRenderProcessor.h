// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "BoidsRenderProcessor.generated.h"

class UBoidsSubsystem;

/**
 * Processor for rendering boids
 */
UCLASS()
class MASSBOIDSGAME_API UBoidsRenderProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
	FMassEntityQuery Entities;

	UPROPERTY(Transient)
	UBoidsSubsystem* BoidsSubsystem;

	UBoidsRenderProcessor(const FObjectInitializer& ObjectInitializer);

	// ~ begin UMassProcessor interface
	virtual void Initialize(UObject& Owner) override;
	virtual void ConfigureQueries() override;
	virtual void Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context) override;
	// ~ end UMassProcessor interface
};
