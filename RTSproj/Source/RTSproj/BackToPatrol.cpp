// MOsipa 2018

#include "BackToPatrol.h"
#include "RTSEnemyUnit.h"
#include "EnemyAIController.h"
#include "GuardTower.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBackToPatrol::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();

	EnemyAIController = Cast<AEnemyAIController>(OwnerComp.GetOwner());
	if (!EnemyAIController) { return EBTNodeResult::Failed; }

	AGuardTower* TowerToLeave = Cast<ARTSEnemyUnit>(EnemyAIController->GetPawn())->GetTower();
	
	if (TowerToLeave)
	{
		ARTSEnemyUnit* EnemyUnit = Cast<ARTSEnemyUnit>(EnemyAIController->GetPawn());
		TowerToLeave->UnitLeft(EnemyUnit);
		EnemyUnit->SetActorHiddenInGame(false);
		EnemyUnit->SetActorEnableCollision(true);
		BlackboardComponent->ClearValue("Alarm");
	}

	return EBTNodeResult::Succeeded;
}