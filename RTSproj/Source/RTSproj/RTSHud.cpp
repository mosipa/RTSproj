// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSHud.h"
#include "GameFramework/PlayerController.h"
#include "RTSprojCharacter.h"

void ARTSHud::DrawHUD()
{
	if (bStartSelecting)
	{
		if (SelectedActors.Num() > 0)
		{
			for (auto Character : SelectedActors)
			{
				Character->Unselect();
			}

			SelectedActors.Empty();
		}

		CurrentPoint = GetMousePosition2D();

		DrawRect(FLinearColor(0.f, 0.f, 1.f, 0.10f), InitialPoint.X, InitialPoint.Y, 
			CurrentPoint.X - InitialPoint.X, CurrentPoint.Y - InitialPoint.Y);

		GetActorsInSelectionRectangle<ARTSprojCharacter>(InitialPoint, CurrentPoint, SelectedActors, false, false);

		for(auto Character : SelectedActors)
		{
			Character->Select();
		}
	}
}

FVector2D ARTSHud::GetMousePosition2D()
{
	float PosX;
	float PosY;

	GetOwningPlayerController()->GetMousePosition(PosX, PosY);
	
	return FVector2D(PosX, PosY);
}

