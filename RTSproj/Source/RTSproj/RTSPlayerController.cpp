// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerController.h"
#include "RTSHud.h"
#include "RTSprojCharacter.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"

ARTSPlayerController::ARTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ARTSPlayerController::BeginPlay()
{
	HUDPtr = Cast<ARTSHud>(GetHUD());
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!InputComponent) { return; }

	InputComponent->BindAction("Select", IE_Pressed, this, &ARTSPlayerController::Select);
	InputComponent->BindAction("Select", IE_Released, this, &ARTSPlayerController::FinishSelecting);
	InputComponent->BindAction("Move", IE_Pressed, this, &ARTSPlayerController::Move);
	InputComponent->BindAction("Knife", IE_Pressed, this, &ARTSPlayerController::Knife);
}

void ARTSPlayerController::Select()
{
	if (!HUDPtr) { return; }
	UE_LOG(LogTemp, Warning, TEXT("Selecting"));
	HUDPtr->InitialPoint = HUDPtr->GetMousePosition2D();
	HUDPtr->bStartSelecting = true;
}

void ARTSPlayerController::FinishSelecting()
{
	if (!HUDPtr) { return; }
	UE_LOG(LogTemp, Warning, TEXT("FinishSelecting"));
	HUDPtr->bStartSelecting = false;
}

void ARTSPlayerController::Move()
{
	if (!HUDPtr) { return; }
	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving"));
		TArray<ARTSprojCharacter*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
		FVector MoveTo = Hit.Location;

		for (auto Actor : SelectedActors)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Actor->GetController(), MoveTo);		
		}
	}
}

void ARTSPlayerController::Knife()
{
	if (!HUDPtr) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSprojCharacter*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;
		
		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit))
		{
			if (Hit.GetActor()->GetClass()->IsChildOf<ARTSprojCharacter>())
			{
				for (auto Actor : SelectedActors)
				{
					if (Actor->GetName().Equals(Hit.GetActor()->GetName()))
					{
						continue; //TODO decide if we want to have friendly fire (if not put break)
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Knife"));
						UAIBlueprintHelperLibrary::SimpleMoveToActor(Actor->GetController(), Hit.GetActor());
						
						//TODO knifing target logic
						float DotProd = Hit.GetActor()->GetDotProductTo(Actor);
						 
						if (DotProd <= 0)
						{
							UE_LOG(LogTemp, Warning, TEXT("Backstab"));
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Frontstab"));
						}
					}
				}
			}
		}
	}
}