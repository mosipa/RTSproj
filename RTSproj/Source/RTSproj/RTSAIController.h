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

	ARTSAIController();

public:
	FORCEINLINE bool IsUnitBusy() { return bUnitBusy; }

	UFUNCTION(BlueprintCallable)
		void Move(FVector MoveTo);
	
	void Knife(FHitResult Hit);

	void FirePistol(FHitResult Hit);

	void PresentYourself();

	void Aid(FHitResult Hit, EUnitState UnitState);

	void EnterBuilding(class ABuilding* Building, class ARTSHud* HUDPtr);

private:
	bool bUnitBusy;

	float Damage;

	AActor* Target;

	FTimerHandle AttackTimerHandle;
	FTimerHandle GettingCloserTimerHandle;
	FTimerHandle AidTimerHandle;
	FTimerHandle MoveTimerHandle;
	FTimerHandle EnterBuildingTimerHandle;

	void PrepareAttack();
	void PrepareToFire();

	UFUNCTION()
		void PerformAttack();

	void PrepareAid();
	
	UFUNCTION()
		void GetCloserForAid();

	UFUNCTION()
		void PerformAid();

	UFUNCTION()
		void PerformMove();

	UFUNCTION()
		void PerformEnterBuilding(class ABuilding* TargetBuilding, class ARTSHud* HUDPtr);

	EUnitState InteralUnitState;

	float AidTime;
	float SelfAidPenalty;
};
