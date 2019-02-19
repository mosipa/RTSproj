// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "StartShooting.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API UStartShooting : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) override;

private:
	UBlackboardComponent* Blackboard = nullptr;
	class AGuardTower* Pawn = nullptr;
};
