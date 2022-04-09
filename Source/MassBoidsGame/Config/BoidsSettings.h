// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassSettings.h"
#include "BoidsSettings.generated.h"

/**
 * Global settings for the Boids system
 */
UCLASS(Config=Game, defaultconfig, dontCollapseCategories)
class MASSBOIDSGAME_API UBoidsSettings : public UMassModuleSettings
{
	GENERATED_BODY()

public:

	/** World origin location to handle boids */
	UPROPERTY(Category="Bounds", Config, BlueprintReadWrite, EditAnywhere)
	FVector Origin;

	/** Alignment factor */
	UPROPERTY(Category="Rules", Config, BlueprintReadWrite, EditAnywhere, Meta=(ClampMin="0.0", ClampMax="1.0", ConsoleVariable="boids.Alignment"))
	float Alignment;

	/** Distance to find other boids for alignment */
	UPROPERTY(Category="Rules", Config, BlueprintReadWrite, EditAnywhere, Meta=(ConsoleVariable="boids.AlignmentDistanceSq"))
	float AlignmentDistanceSquared;

	/** Separation factor */
	UPROPERTY(Category="Rules", Config, BlueprintReadWrite, EditAnywhere, Meta=(ClampMin="0.0", ClampMax="1.0", ConsoleVariable="boids.Separation"))
	float Separation;

	/** Distance to find other boids for separation */
	UPROPERTY(Category="Rules", Config, BlueprintReadWrite, EditAnywhere, Meta=(ConsoleVariable="boids.SeparationDistanceSq"))
	float SeparationDistanceSquared;

	/** Cohesion factor */
	UPROPERTY(Category="Rules", Config, BlueprintReadWrite, EditAnywhere, Meta=(ClampMin="0.0", ClampMax="1.0", ConsoleVariable="boids.Cohesion"))
	float Cohesion;

	/** Distance to find other boids for cohesion */
	UPROPERTY(Category="Rules", Config, BlueprintReadWrite, EditAnywhere, Meta=(ConsoleVariable="boids.CohesionDistanceSq"))
	float CohesionDistanceSquared;
	
	/** World Size of the world for boids */
	UPROPERTY(Category="Bounds", Config, BlueprintReadWrite, EditAnywhere)
	float Extent;

	/** Distance from the bounds to start turning back boids */
	UPROPERTY(Category="Bounds", Config, BlueprintReadWrite, EditAnywhere)
	float TurnBackOffset;

	/** Rate at which to turn back boids */
	UPROPERTY(Category="Bounds", Config, BlueprintReadWrite, EditAnywhere)
	float TurnBackRate;

	/** The Sqrt size of each grid */
	UPROPERTY(Category="Grid", Config, BlueprintReadWrite, EditAnywhere)
	float GridSize;
	
	UBoidsSettings(const FObjectInitializer& ObjectInitializer);
};
