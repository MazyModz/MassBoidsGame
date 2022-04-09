// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassCommonTypes.h"
#include "BoidsSpawnTag.generated.h"

/**
 * Tag for Boids Entities that were spawned this frame
 */
USTRUCT()
struct MASSBOIDSGAME_API FBoidsSpawnTag : public FMassTag
{
	GENERATED_BODY()
};
