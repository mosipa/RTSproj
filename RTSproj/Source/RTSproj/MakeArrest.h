// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MakeArrest.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API UMakeArrest : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) override;

private:
	UBlackboardComponent* BlackboardComponent;
	APawn* Pawn;

	bool bEnemyPrevLocationSet = false;
	bool bEnemyMoved = false;
	FVector EnemyPrevLocation;

	bool IsEnemyPrevLocationSet();
	void SetEnemyPrevLocation(UBlackboardComponent* Blackboard);
};
