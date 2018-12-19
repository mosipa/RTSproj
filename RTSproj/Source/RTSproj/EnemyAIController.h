// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	AEnemyAIController();

public:
	virtual void BeginPlay() override;
	
private:
	class UBlackboardComponent* BlackboardComponent;
	class UBlackboardData* BlackboardAsset;
	class UBehaviorTree* BehaviorTree;

	virtual void Possess(APawn* Pawn) override;
};
