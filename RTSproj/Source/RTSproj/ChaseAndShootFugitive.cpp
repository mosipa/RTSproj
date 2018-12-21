// MOsipa 2018

#include "ChaseAndShootFugitive.h"

EBTNodeResult::Type UChaseAndShootFugitive::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("Fugitive trying to escape"));

	return EBTNodeResult::Succeeded;
}


