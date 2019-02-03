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
	class AGuardTower* ClosestTower;
	FTimerHandle ExitingTimer;

	UFUNCTION()
		void GettingOut();

	void GetClosestTower();

	bool bHasTimerStarted = false;
	bool bHasClosestTowerSet = false;

	//TODO find distance between most west and most east wall of level
	float DistanceMin = 10000.f;
};
