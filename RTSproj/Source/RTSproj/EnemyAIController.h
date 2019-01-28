// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIModule/Classes/Perception/AIPerceptionTypes.h"
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

	FORCEINLINE TArray<class AGuardTower*> GetGuardTowersInLevel() const { return GuardTowersInLevel; }

private:
	class UBlackboardComponent* BlackboardComponent;
	class UBlackboardData* BlackboardAsset;
	class UBehaviorTree* BehaviorTree;

	class UAIPerceptionComponent* AIPerceptionComponent;
	class UAISenseConfig_Sight* Sight;

	virtual void Possess(APawn* Pawn) override;

	//Adding to GuardsTowersInLevel all spawned GuardTowers
	void GetAllGuardTowers();

	UFUNCTION()
		void OnTargetPerceptionUpdated(AActor* SensedActor, FAIStimulus Stimulus);

	TArray<class AGuardTower*> GuardTowersInLevel;
};
