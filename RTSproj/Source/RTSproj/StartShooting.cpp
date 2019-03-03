// MOsipa 2018

#include "StartShooting.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GuardTowerController.h"
#include "GuardTower.h"
#include "RTSPlayerUnit.h"

EBTNodeResult::Type UStartShooting::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();

	Pawn = Cast<AGuardTower>(Cast<AGuardTowerController>(OwnerComp.GetOwner())->GetPawn());
	if (!Pawn) { return EBTNodeResult::Failed; }

	//If target exists
	ARTSPlayerUnit* PlayerToShootAt = Cast<ARTSPlayerUnit>(Blackboard->GetValueAsObject("PlayerUnitKey"));
	if(!PlayerToShootAt) { return EBTNodeResult::Failed; }

	//If there's noone inside tower
	if (!Pawn->IsAnybodyInside())
	{
		//Clear blackboard values
		Blackboard->ClearValue("Shoot");
		Blackboard->ClearValue("PlayerUnitKey");

		return EBTNodeResult::Aborted;
	}

	//Fire from peek
	Pawn->OpenFire(PlayerToShootAt);

	return EBTNodeResult::Succeeded;
}