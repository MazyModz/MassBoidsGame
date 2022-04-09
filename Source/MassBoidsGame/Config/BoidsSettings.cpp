// Copyright Dennis Andersson. All Rights Reserved.


#include "BoidsSettings.h"

UBoidsSettings::UBoidsSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Origin(FVector::ZeroVector)
	, Alignment(0.5f)
	, AlignmentDistanceSquared(500.f * 500.f)
	, Separation(1.f)
	, SeparationDistanceSquared(100.f * 100.f)
	, Cohesion(0.5f)
	, CohesionDistanceSquared(500.f * 500.f)
	, Extent(10000.f)
	, TurnBackOffset(500.f)
	, TurnBackRate(20.f)
	, GridSize(2500.f)
{
}
