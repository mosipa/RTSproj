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

	void WoundCleansing();

	void Healing();

private:
	bool bSomeoneToStab;
	bool bSomeoneToShoot;
	bool bSomeoneToCleanse;
	bool bSomeoneToHeal;

	AActor* Target;

	float GetDistance(FVector A, FVector B);

	void PerformCleansing();

	UFUNCTION()
		void Cleansing();

	FTimerHandle CleansingTimerHandle;
};
