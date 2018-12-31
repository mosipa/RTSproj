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
	AActor* Target;
	
	//TODO get location of prison from map
	FVector PRISON_LOCATION = FVector(-380.f, 0.f, 170.f);
};
