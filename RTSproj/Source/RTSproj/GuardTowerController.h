// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIModule/Classes/Perception/AIPerceptionTypes.h"
#include "GuardTowerController.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API AGuardTowerController : public AAIController
{
	GENERATED_BODY()

	AGuardTowerController();

private:
	class UAIPerceptionComponent* AIPerceptionComponent;
	class UAISenseConfig_Sight* Sight;

	UFUNCTION()
		void OnTargetPerceptionUpdated(AActor* SensedActor, FAIStimulus Stimulus);
};
