// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "Config/BoidsSettings.h"
#include "BoidsBoundsProcessor.generated.h"

/**
 * Keeps boids inside of the world bounds
 */
UCLASS()
class MASSBOIDSGAME_API UBoidsBoundsProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	
	FMassEntityQuery Entities;

	UPROPERTY(Transient)
	UBoidsSettings* BoidsSettings;
	
	UBoidsBoundsProcessor(const FObjectInitializer& ObjectInitializer);

	// ~ begin UMassProcessor interface
	virtual void Initialize(UObject& Owner) override;
	virtual void ConfigureQueries() override;
	virtual void Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context) override;
	// ~ end UMassProcessor interface
};
