// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSHud.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerUnit.h"

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

		TArray<class ARTSPlayerUnit*> ActorsToSelect;
		GetActorsInSelectionRectangle<ARTSPlayerUnit>(InitialPoint, CurrentPoint, ActorsToSelect, false, false);

		//Only select actors that aren't dead
		for(auto Character : ActorsToSelect)
		{
			if (!Character->IsCharacterDead()
				&& !Character->IsCharacterInBuilding())
			{
				SelectedActors.Add(Character);
			}
		}

		for (auto Character : SelectedActors)
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

void ARTSHud::RemoveUnitFromSelection()
{
	TArray<class ARTSPlayerUnit*> ActorsToSelect;

	for (auto Character : SelectedActors)
	{
		//If character is in building - unselect
		if (Character->IsCharacterInBuilding())
		{
			Character->Unselect();
		}
		//If not keep selection
		else
		{
			ActorsToSelect.Add(Character);
		}
	}
	
	SelectedActors = ActorsToSelect;
}
