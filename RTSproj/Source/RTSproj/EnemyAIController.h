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

private:
	class UBlackboardComponent* BlackboardComponent;
	class UBlackboardData* BlackboardAsset;
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere)
	class UAIPerceptionComponent* AIPerceptionComponent;
	UPROPERTY(VisibleAnywhere)
	class UAISenseConfig_Sight* Sight;

	virtual void Possess(APawn* Pawn) override;

	UFUNCTION()
		void OnTargetPerceptionUpdated(AActor* SensedActor, FAIStimulus Stimulus);
};
