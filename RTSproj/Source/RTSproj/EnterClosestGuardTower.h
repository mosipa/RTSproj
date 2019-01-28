// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnterClosestGuardTower.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API UEnterClosestGuardTower : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) override;

private:
	UBlackboardComponent* BlackboardComponent;
	class AEnemyAIController* EnemyAIController;
};