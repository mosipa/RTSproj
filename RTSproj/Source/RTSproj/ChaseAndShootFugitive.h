// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ChaseAndShootFugitive.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API UChaseAndShootFugitive : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) override;

private:
	UBlackboardComponent* BlackboardComponent;
	APawn* Pawn;

	//TODO create class with math stuff or get one from UE4 libraries
	float GetDistance(FVector A, FVector B);
};
