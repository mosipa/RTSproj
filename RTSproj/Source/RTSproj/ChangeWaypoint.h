// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ChangeWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API UChangeWaypoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) override;

private:
	TArray<AActor*> PatrolPoints;
	APawn* Pawn;
	UBlackboardComponent* Blackboard;

	int32 Index;

	void GetPatrolPoints(UBehaviorTreeComponent& OwnerComp);
	void SetNextWaypoint();
	void SetNextIndex();
	void SetPatrolSpeed(APawn* Pawn);
};