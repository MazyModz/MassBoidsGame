// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "Config/BoidsSettings.h"
#include "BoidsRuleProcessor.generated.h"

/**
 * Processor that apply the rules of boids
 */
UCLASS()
class MASSBOIDSGAME_API UBoidsRuleProcessor : public UMassProcessor
{
	GENERATED_BODY()

	FMassEntityQuery Entities;

	UPROPERTY(Transient)
	UBoidsSettings* BoidsSettings;
	
	TArray<FVector> BoidAlignments;
	TArray<FVector> BoidSeparations;
	TArray<FVector> BoidCohesions;

	TArray<TArray<int32>> BoidsGrid;
	TArray<int32> BoidsGridIndex;
	TArray<int32> BoidsPerGrid;

	UBoidsRuleProcessor(const FObjectInitializer& ObjectInitializer);

	// ~ begin UMassProcessor interface
	virtual void Initialize(UObject& Owner) override;
	virtual void ConfigureQueries() override;
	virtual void Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context) override;
	// ~ end UMassProcessor interface

	void SetupBoidsGrid(TArray<const FVector*>& BoidLocations, const int32 NumBoids);
	void RunBoidsAlignment(TArray<const FVector*>& BoidLocations, const int32 NumBoids);
	void RunBoidsSeparation(TArray<const FVector*>& BoidLocations, const int32 NumBoids);
	void RunBoidsCohesion(TArray<const FVector*>& BoidLocations, TArray<FVector*>& BoidVelocities, const int32 NumBoids);
};
