// MOsipa 2018

#include "ChangeWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAIController.h"
#include "RTSEnemyUnit.h"
#include "EnemyAIController.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UChangeWaypoint::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();
	Pawn = Cast<APawn>(Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn());

	GetPatrolPoints(OwnerComp);
	if (PatrolPoints.Num() == 0) { return EBTNodeResult::Aborted; }

	SetNextWaypoint();
	SetNextIndex();
	SetPatrolSpeed(Pawn);

	return EBTNodeResult::Succeeded;
}

void UChangeWaypoint::GetPatrolPoints(UBehaviorTreeComponent& OwnerComp)
{
	auto BTOwner = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();

	if (BTOwner) { PatrolPoints = Cast<ARTSEnemyUnit>(BTOwner)->GetPatrolPoints(); }
}

void UChangeWaypoint::SetNextWaypoint()
{
	Index = Blackboard->GetValueAsInt("IndexKey");
	auto ArrayEl = PatrolPoints[Index];
	Blackboard->SetValueAsVector(FName("NextWaypoint"), ArrayEl->GetActorLocation());
}

void UChangeWaypoint::SetNextIndex()
{
	auto NewIndex = (Index + 1) % PatrolPoints.Num();
	Blackboard->SetValueAsInt(FName("IndexKey"), NewIndex);
}

void UChangeWaypoint::SetPatrolSpeed(APawn* Pawn)
{
	Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 300.f;
}
