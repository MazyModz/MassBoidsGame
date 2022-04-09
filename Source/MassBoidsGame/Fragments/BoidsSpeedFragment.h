// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassCommonTypes.h"
#include "BoidsSpeedFragment.generated.h"

USTRUCT(BlueprintType)
struct MASSBOIDSGAME_API FBoidsSpeedFragment : public FMassFragment
{
	GENERATED_BODY()

	/** Maximum movement speed of boids */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta=(ClampMin="1.0", UIMin="1.0", ForceUnits="cm/s^2"))
	float MaxSpeed;

	FBoidsSpeedFragment()
		: MaxSpeed(300.f)
	{
	}
};
