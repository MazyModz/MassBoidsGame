// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "BoidsMoveProcessor.generated.h"

/**
 * Processor that Updates the Location of Boids based on Velocity
 */
UCLASS()
class MASSBOIDSGAME_API UBoidsMoveProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:

	FMassEntityQuery Entities;
	
	UBoidsMoveProcessor(const FObjectInitializer& ObjectInitializer);

	// ~ begin UMassProcessor interface
	virtual void ConfigureQueries() override;
	virtual void Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context) override;
	// ~ end UMassProcessor interface
};
