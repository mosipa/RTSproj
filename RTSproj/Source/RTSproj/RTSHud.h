// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTSHud.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API ARTSHud : public AHUD
{
	GENERATED_BODY()
	
private:
	TArray<class ARTSprojCharacter*> SelectedActors;

public:
	FVector2D InitialPoint;

	FVector2D CurrentPoint;

	FVector2D GetMousePosition2D();

	virtual void DrawHUD() override;

	bool bStartSelecting = false;

	FORCEINLINE TArray<class ARTSprojCharacter*> GetSelectedActors() const {return SelectedActors;}

};
