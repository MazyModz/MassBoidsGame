// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "BoidsSpawnProcessor.generated.h"

class UBoidsSubsystem;

/**
 * Processor that Updates the Location of Boids based on Velocity
 */
UCLASS()
class MASSBOIDSGAME_API UBoidsSpawnProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:

	FMassEntityQuery Entities;

	UPROPERTY(Transient)
	UBoidsSubsystem* BoidsSubsystem;
	
	UBoidsSpawnProcessor(const FObjectInitializer& ObjectInitializer);

	// ~ begin UMassProcessor interface
	virtual void Initialize(UObject& Owner) override;
	virtual void ConfigureQueries() override;
	virtual void Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context) override;
	// ~ end UMassProcessor interface
};
