// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassCommonTypes.h"
#include "BoidsMeshFragment.generated.h"

USTRUCT(BlueprintType)
struct MASSBOIDSGAME_API FBoidsMeshFragment : public FMassSharedFragment
{
	GENERATED_BODY()

	/** The render mesh for a boid */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* BoidMesh;

	FBoidsMeshFragment()
		: BoidMesh(nullptr)
	{
	}
};
