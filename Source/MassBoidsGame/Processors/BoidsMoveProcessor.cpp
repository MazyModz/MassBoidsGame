// Copyright Dennis Andersson. All Rights Reserved.


#include "BoidsMoveProcessor.h"
#include "BoidsTypes.h"

#include "BoidsRuleProcessor.h"
#include "MassMovementFragments.h"
#include "Fragments/BoidsLocationFragment.h"
#include "Fragments/BoidsSpeedFragment.h"


UBoidsMoveProcessor::UBoidsMoveProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ExecutionOrder.ExecuteAfter.Add(UBoidsRuleProcessor::StaticClass()->GetFName());
	ExecutionOrder.ExecuteInGroup = MassBoidsGame::ProcessorGroupNames::Boids;
}

void UBoidsMoveProcessor::ConfigureQueries()
{
	Entities
		.AddRequirement<FBoidsLocationFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All)
		.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All)
		.AddRequirement<FBoidsSpeedFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All);
}

void UBoidsMoveProcessor::Execute(UMassEntitySubsystem& EntitySubsystem, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsMoveProcessor);

	Entities.ParallelForEachEntityChunk(EntitySubsystem, Context, [] (FMassExecutionContext& Context)
	{
		const TArrayView<FBoidsLocationFragment>& Locations = Context.GetMutableFragmentView<FBoidsLocationFragment>();
		const TArrayView<FMassVelocityFragment>& Velocities = Context.GetMutableFragmentView<FMassVelocityFragment>();
		const TConstArrayView<FBoidsSpeedFragment>& Speeds = Context.GetFragmentView<FBoidsSpeedFragment>();
		
		const float DeltaTime = Context.GetDeltaTimeSeconds();
		const int32 NumEntities = Context.GetNumEntities();
		
		for (int32 Ndx = 0; Ndx < NumEntities; Ndx++)
		{
			// Limit speed to MaxSpeed
			Velocities[Ndx].Value = (Velocities[Ndx].Value / Velocities[Ndx].Value.Size()) * Speeds[Ndx].MaxSpeed;
			// Update the location based on Velocity
			Locations[Ndx].Location += Velocities[Ndx].Value * DeltaTime;
		}
	});
}
