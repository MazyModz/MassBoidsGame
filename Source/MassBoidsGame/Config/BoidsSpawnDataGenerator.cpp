// Copyright Dennis Andersson. All Rights Reserved.


#include "BoidsSpawnDataGenerator.h"
#include "BoidsSettings.h"
#include "Processors/BoidsSpawnProcessor.h"

void UBoidsSpawnDataGenerator::Generate(UObject& QueryOwner, TConstArrayView<FMassSpawnedEntityType> EntityTypes, int32 Count, FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const
{
	const UBoidsSettings* Settings = GetDefault<UBoidsSettings>();
	
	const FVector MinExtent = FVector(-(Settings->Extent / 2.f));
	const FVector MaxExtent = FVector((Settings->Extent / 2.f));
	const FBox BoundingBox = FBox(MinExtent - Settings->TurnBackOffset, MaxExtent + Settings->TurnBackOffset);
	
	TArray<FMassEntitySpawnDataGeneratorResult> Results;
	BuildResultsFromEntityTypes(Count, EntityTypes, Results);
	
	for (FMassEntitySpawnDataGeneratorResult& Result : Results)
	{
		Result.SpawnDataProcessor = UBoidsSpawnProcessor::StaticClass();
		Result.SpawnData.InitializeAs<FMassTransformsSpawnData>();
		FMassTransformsSpawnData& Transforms = Result.SpawnData.GetMutable<FMassTransformsSpawnData>();
		Transforms.Transforms.Reserve(Result.NumEntities);
		
		for (int32 Ndx = 0; Ndx < Result.NumEntities; Ndx++)
		{
			const FRotator RandRot = FRotator(FMath::FRandRange(-180.f, 180.f),FMath::FRandRange(-180.f, 180.f), FMath::FRandRange(-180.f, 180.f));
			const FVector RandPoint = FMath::RandPointInBox(BoundingBox);
			Transforms.Transforms.Emplace(RandRot, RandPoint, FVector::ZeroVector);
		}
	}

	FinishedGeneratingSpawnPointsDelegate.Execute(Results);
}
