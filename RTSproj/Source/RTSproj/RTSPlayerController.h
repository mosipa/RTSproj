// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ARTSPlayerController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetupInputComponent() override;

	class ARTSHud* HUDPtr;

protected:
	void Select();

	void FinishSelecting();

	void Move();

	void Knife();

	void Pistol();

	void Aid();

private:
	bool bSomeoneToStab;
	bool bSomeoneToShoot;
	bool bSomeoneToAid;

	AActor* Target;

	float GetDistance(FVector A, FVector B);

	void PerformAid();

	UFUNCTION()
		void Cleansing();

	UFUNCTION()
		void Healing();

	FTimerHandle CleansingTimerHandle;
	FTimerHandle HealingTimerHandle;

	enum class EAidState : uint8
	{
		Cleansing,
		Healing
	};

	EAidState AidState;

	float CleansingTime;
	float HealingTime;
	float SelfAidTime;
};
