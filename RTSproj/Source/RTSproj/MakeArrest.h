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

	bool bPlayerUnitPrevLocationSet = false;
	bool bPlayerUnitMoved = false;
	bool bArrested = false;
	FVector PlayerUnitPrevLocation;
	
	//TODO get location of prison from map
	FVector PRISON_LOCATION = FVector(-380.f, 0.f, 170.f);

	void SetPlayerUnitPrevLocation(UBlackboardComponent* Blackboard);

	//TODO create class with math stuff or get one from UE4 libraries
	float GetDistance(FVector A, FVector B);
};
