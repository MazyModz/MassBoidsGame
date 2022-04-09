// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassCommonTypes.h"
#include "BoidsLocationFragment.generated.h"

USTRUCT()
struct MASSBOIDSGAME_API FBoidsLocationFragment : public FMassFragment
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector Location;

	FBoidsLocationFragment()
		: Location(ForceInitToZero)
	{
	}
};
