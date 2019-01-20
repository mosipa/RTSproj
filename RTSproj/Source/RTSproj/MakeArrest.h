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
	class ARTSPlayerUnit* Target;
	class APrison* Prison;

	FVector PRISON_LOCATION;
	
	UFUNCTION()
		void PutCollisionBackOn();

	FTimerHandle CollisionTimerHandle;

	bool bCollisionToggle = false;
	bool bFindPrisonLocation = false;

	void FindPrisonLocation();
};
