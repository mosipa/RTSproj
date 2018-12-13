// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RTSAIController.generated.h"

/**
 * 
 */
enum class EUnitState : uint8
{
	Knifing,
	FiringGun,
	Cleansing,
	Healing
};

UCLASS()
class RTSPROJ_API ARTSAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	void Move(FVector MoveTo);
	
	void Knife(FHitResult Hit);

	void FirePistol(FHitResult Hit);

	void PresentYourself();

	void Aid(FHitResult Hit, EUnitState UnitState);

private:
	float Damage;

	AActor* Target;

	float GetDistance(FVector A, FVector B);

	FTimerHandle AttackTimerHandle;
	FTimerHandle GettingCloserTimerHandle;
	FTimerHandle AidTimerHandle;

	void PrepareAttack();
	void PrepareToFire();

	UFUNCTION()
		void PerformAttack();

	void PrepareAid();
	
	UFUNCTION()
		void GetCloserForAid();

	UFUNCTION()
		void PerformAid();

	EUnitState InteralUnitState;

	float AidTime;
	float SelfAidPenalty;
};
