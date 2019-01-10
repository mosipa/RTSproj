// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
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

	UFUNCTION(BlueprintCallable)
		TArray<class ARTSPlayerUnit*> GetAllSelectedUnits();

protected:
	void Select();

	void FinishSelecting();

	void Move();

	void Knife();

	void Pistol();

	void Aid();

	void ZoomIn();

	void ZoomOut();

private:
	class ARTSHud* HUDPtr;

	void MoveCamera();

	class UUserWidget* UserWidget;

	ConstructorHelpers::FClassFinder<class UUserWidget>* thisUser;
};
