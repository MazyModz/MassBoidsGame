// Copyright Dennis Andersson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"


#include "Fragments/BoidsMeshFragment.h"
#include "Fragments/BoidsSpeedFragment.h"

#include "BoidsTrait.generated.h"

/**
 * Config Template for Boid Entities
 */
UCLASS(BlueprintType)
class MASSBOIDSGAME_API UBoidsTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
	
	UPROPERTY(Category="Boids", EditAnywhere)
	FBoidsSpeedFragment Speed;

	UPROPERTY(Category="Boids", EditAnywhere)
	FBoidsMeshFragment Mesh;
	
	// ~ begin UMassEntityTraitBase interface
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, UWorld& World) const override;
	// ~ end UMassEntityTraitBase interface
};
