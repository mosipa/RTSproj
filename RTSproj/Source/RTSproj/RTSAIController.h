// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RTSAIController.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API ARTSAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	void Move(FVector MoveTo);
	
	void Knife(FHitResult Hit);

	void PresentYourself();

private:
	float Damage;

	AActor* Target;

	float GetDistance(FVector A, FVector B);

	FTimerHandle KnifeTimerHandle;

	void PrepareAttack();
	
	UFUNCTION()
		void PerformAttack();
};
