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

	UFUNCTION(BlueprintCallable)
		void RemoveMoveBinding();
	
	UFUNCTION(BlueprintCallable)
		void AddBindingBack();

	UFUNCTION(BlueprintCallable)
		void ChangeBinding(int32 BindingIndex);

	UFUNCTION(BlueprintCallable)
		void SetAidValue(bool bNewAidValue);

	UFUNCTION(BlueprintCallable)
		class APlayersHideout* GetSelectedBuilding();

protected:
	void Select();

	void FinishSelecting();

	void LeftMouseButtonActions();

	void Knife();

	void Pistol();

	void Aid();

	void ZoomIn();

	void ZoomOut();

	void ReleaseUnits();

	void GetBuildingCamera();

	void ToggleTransparency();

private:
	class ARTSHud* HUDPtr;

	void MoveCamera();

	FInputActionBinding RemovedMoveBinding;

	bool bRemovedBinding;
	bool bAidValue;

	class APlayersHideout* HideoutPtr;

	TSubclassOf <class UUserWidget > UWClass;
	class UUserWidget* UserWidget;

	const int32 LEFTMOUSEBUTTONACTION_ID = 11;

	int32 CurrentIndex = 0;
	bool bDisableInputs;

	FVector PrevCamLocation;
	FRotator PrevCamRotation;

	bool bChangedMaterial;
};