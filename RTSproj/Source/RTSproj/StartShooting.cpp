// MOsipa 2018

#include "StartShooting.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GuardTowerController.h"
#include "GuardTower.h"
#include "RTSPlayerUnit.h"

EBTNodeResult::Type UStartShooting::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();

	UE_LOG(LogTemp, Warning, TEXT("%s in StartShooting"), *(Pawn->GetName()));
	Pawn = Cast<AGuardTower>(Cast<AGuardTowerController>(OwnerComp.GetOwner())->GetPawn());
	if (!Pawn) { return EBTNodeResult::Failed; }

	ARTSPlayerUnit* PlayerToShootAt = Cast<ARTSPlayerUnit>(Blackboard->GetValueAsObject("PlayerUnitKey"));
	if(PlayerToShootAt) { return EBTNodeResult::Failed; }
	Pawn->PrepareToFire(PlayerToShootAt);
	return EBTNodeResult::Succeeded;
}