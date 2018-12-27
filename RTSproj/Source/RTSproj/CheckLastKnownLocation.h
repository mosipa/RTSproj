// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CheckLastKnownLocation.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API UCheckLastKnownLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) override;

private:
	UBlackboardComponent* BlackboardComponent;
	class AEnemyAIController* EnemyAIController;
	APawn* Pawn;
	FVector LastKnownLocation;

	float GetDistance(FVector A, FVector B);
};
