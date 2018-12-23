// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerController.h"
#include "RTSHud.h"
#include "RTSCharacter.h"
#include "RTSPlayerUnit.h"
#include "Engine/World.h"
#include "RTSAIController.h"
#include "Engine/Classes/Camera/CameraComponent.h"

ARTSPlayerController::ARTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;
}

void ARTSPlayerController::BeginPlay()
{
	HUDPtr = Cast<ARTSHud>(GetHUD());
}

void ARTSPlayerController::Tick(float DeltaTime)
{
	//Update position of camera every second
	MoveCamera();
}

void ARTSPlayerController::MoveCamera()
{
	if (!HUDPtr) { return; }

	//Current position of cursor
	FVector2D MousePosition = HUDPtr->GetMousePosition2D();

	//Get size of viewport
	int32 OutViewportSizeX; int32 OutViewportSizeY;
	GetViewportSize(OutViewportSizeX, OutViewportSizeY);

	//Get our pawn (camera) current location
	FVector ActorLocation = this->GetPawn()->GetActorLocation();
	
	//If cursor is on the edge, slowly move camera in that direction
	if (MousePosition.X < (float)OutViewportSizeX * 0.05)
	{
		ActorLocation.Y -= 12.f;
		this->GetPawn()->SetActorLocation(ActorLocation);
	}
	if (MousePosition.X > (float)OutViewportSizeX * 0.95)
	{
		ActorLocation.Y += 12.f;
		this->GetPawn()->SetActorLocation(ActorLocation);
	}
	if (MousePosition.Y < (float)OutViewportSizeY * 0.05)
	{
		ActorLocation.X += 12.f;
		this->GetPawn()->SetActorLocation(ActorLocation);
	}
	if (MousePosition.Y > (float)OutViewportSizeY * 0.95)
	{
		ActorLocation.X -= 12.f;
		this->GetPawn()->SetActorLocation(ActorLocation);
	}
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!InputComponent) { return; }

	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ARTSPlayerController::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ARTSPlayerController::ZoomOut);

	InputComponent->BindAction("Select", IE_Pressed, this, &ARTSPlayerController::Select);
	InputComponent->BindAction("Select", IE_Released, this, &ARTSPlayerController::FinishSelecting);
	InputComponent->BindAction("Move", IE_Pressed, this, &ARTSPlayerController::Move);
	InputComponent->BindAction("Knife", IE_Pressed, this, &ARTSPlayerController::Knife);
	InputComponent->BindAction("Pistol", IE_Pressed, this, &ARTSPlayerController::Pistol);
	InputComponent->BindAction("Aid", IE_Pressed, this, &ARTSPlayerController::Aid);
}

void ARTSPlayerController::ZoomIn()
{
	FVector CameraLocation = this->GetPawn()->GetActorLocation();
	CameraLocation.Z -= 20.f;
	this->GetPawn()->SetActorLocation(CameraLocation);
}

void ARTSPlayerController::ZoomOut()
{
	FVector CameraLocation = this->GetPawn()->GetActorLocation();
	CameraLocation.Z += 20.f;
	this->GetPawn()->SetActorLocation(CameraLocation);
}

void ARTSPlayerController::Select()
{
	if (!HUDPtr) { return; }
	HUDPtr->InitialPoint = HUDPtr->GetMousePosition2D();
	HUDPtr->bStartSelecting = true;
}

void ARTSPlayerController::FinishSelecting()
{
	if (!HUDPtr) { return; }
	HUDPtr->bStartSelecting = false;
}

void ARTSPlayerController::Move()
{
	if (!HUDPtr) { return; }
	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSPlayerUnit*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
		FVector MoveTo = Hit.Location;

		for (auto Actor : SelectedActors)
		{
			//If character was selected but died in process dont give him an order
			if (!Cast<ARTSPlayerUnit>(Actor)->IsCharacterDead())
			{
				Cast<ARTSAIController>(Actor->GetController())->Move(MoveTo);
			}
		}
	}
}

void ARTSPlayerController::Knife()
{
	if (!HUDPtr) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSPlayerUnit*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit))
		{
			for (auto Actor : SelectedActors)
			{
				//If character was selected but died in process dont give him an order
				if (!Cast<ARTSPlayerUnit>(Actor)->IsCharacterDead())
				{
					Cast<ARTSAIController>(Actor->GetController())->Knife(Hit);
				}
			}
		}
	}
}

void ARTSPlayerController::Pistol()
{
	if (!HUDPtr) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSPlayerUnit*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit))
		{
			for (auto Actor : SelectedActors)
			{
				//If character was selected but died in process dont give him an order
				if (!Cast<ARTSPlayerUnit>(Actor)->IsCharacterDead())
				{
					Cast<ARTSAIController>(Actor->GetController())->FirePistol(Hit);
				}
			}
		}
	}
}

void ARTSPlayerController::Aid()
{
	if (!HUDPtr) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSPlayerUnit*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit))
		{
			for (auto Actor : SelectedActors)
			{
				if (!Cast<ARTSPlayerUnit>(Actor)->IsCharacterDead())
				{
					if (this->WasInputKeyJustPressed(FKey(FName("H"))))
					{
						Cast<ARTSAIController>(Actor->GetController())->Aid(Hit, EUnitState::Healing);
					}
					else if (this->WasInputKeyJustPressed(FKey(FName("C"))))
					{
						Cast<ARTSAIController>(Actor->GetController())->Aid(Hit, EUnitState::Cleansing);
					}
				}
			}
		}
	}
}