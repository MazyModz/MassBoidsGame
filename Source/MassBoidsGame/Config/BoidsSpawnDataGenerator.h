// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntitySpawnDataGeneratorBase.h"
#include "BoidsSpawnDataGenerator.generated.h"

/**
 * Class that generated spawn points for boids. Currently just random points inside the world bounds
 */
UCLASS(BlueprintType)
class MASSBOIDSGAME_API UBoidsSpawnDataGenerator : public UMassEntitySpawnDataGeneratorBase
{
	GENERATED_BODY()

public:

	// ~ begin UMassEntitySpawnDataGeneratorBase interface
	virtual void Generate(UObject& QueryOwner, TConstArrayView<FMassSpawnedEntityType> EntityTypes, int32 Count, FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const override;
	// ~ end UMassEntitySpawnDataGeneratorBase interface
};
