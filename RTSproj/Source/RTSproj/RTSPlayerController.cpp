// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerController.h"
#include "RTSHud.h"
#include "RTSCharacter.h"
#include "RTSPlayerUnit.h"
#include "Engine/World.h"
#include "RTSAIController.h"
#include "Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"

ARTSPlayerController::ARTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;

	bRemovedBinding = false;
	bAidValue = false;
	
	ConstructorHelpers::FClassFinder<UUserWidget> CHUserHUD_BP(TEXT("/Game/Blueprints/UserHUD_BP"));
	if (CHUserHUD_BP.Succeeded())
	{
		UWClass = CHUserHUD_BP.Class;
	}
}

void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	HUDPtr = Cast<ARTSHud>(GetHUD());

	//Set RemovedMoveBinding in BeginPlay because binding changes
	//And during changes it may override that variable which contains original method
	//So I set it here to prevent that
	//At least to the moment I find out a better way 
	RemovedMoveBinding = InputComponent->GetActionBinding(8);

	//Set UserHUD
	UserWidget = CreateWidget<UUserWidget>(this, UWClass);
	if (UserWidget)
	{
		UserWidget->AddToPlayerScreen();
	}
}

void ARTSPlayerController::Tick(float DeltaTime)
{
	//Update position of camera every tick
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
		ActorLocation.Y -= 18.f;
		this->GetPawn()->SetActorLocation(ActorLocation);
	}
	if (MousePosition.X > (float)OutViewportSizeX * 0.95)
	{
		ActorLocation.Y += 18.f;
		this->GetPawn()->SetActorLocation(ActorLocation);
	}
	if (MousePosition.Y < (float)OutViewportSizeY * 0.05)
	{
		ActorLocation.X += 18.f;
		this->GetPawn()->SetActorLocation(ActorLocation);
	}
	if (MousePosition.Y > (float)OutViewportSizeY * 0.95)
	{
		ActorLocation.X -= 18.f;
		this->GetPawn()->SetActorLocation(ActorLocation);
	}
}

TArray<class ARTSPlayerUnit*> ARTSPlayerController::GetAllSelectedUnits()
{
	if (!HUDPtr) { return TArray<class ARTSPlayerUnit*>(); }
	return HUDPtr->GetSelectedActors();
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!InputComponent) { return; }

	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ARTSPlayerController::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ARTSPlayerController::ZoomOut);
	
	InputComponent->BindAction("Select", IE_Pressed, this, &ARTSPlayerController::Select);
	InputComponent->BindAction("Select", IE_Released, this, &ARTSPlayerController::FinishSelecting);
	InputComponent->BindAction("Knife", IE_Pressed, this, &ARTSPlayerController::Knife);
	InputComponent->BindAction("Pistol", IE_Pressed, this, &ARTSPlayerController::Pistol);
	InputComponent->BindAction("Aid", IE_Pressed, this, &ARTSPlayerController::Aid);
	InputComponent->BindAction("Cancel", IE_Pressed, this, &ARTSPlayerController::AddBindingBack);
	InputComponent->BindAction("Move", IE_Pressed, this, &ARTSPlayerController::Move);
}

void ARTSPlayerController::RemoveMoveBinding()
{
	if (!InputComponent) { return; }

	//In case it's been already removed - dont do it again
	if (!bRemovedBinding)
	{
		//8 - Move - LeftMouseButton
		//TODO find a way to get FInputActionBinding with a NAME, not int
		//use BindingToRemove.GetActionName().ToString() to get name, maybe there's something in it
		//compare to all bindings and find the one (MOVE) to remove
		InputComponent->RemoveActionBinding(8);
		bRemovedBinding = true;
	}
}

void ARTSPlayerController::AddBindingBack()
{
	if (!InputComponent) { return; }

	//In case action picked up from User_HUD is over
	//OR it got canceled - then bring back old binding
	if (bRemovedBinding)
	{
		InputComponent->AddActionBinding(RemovedMoveBinding);
		bRemovedBinding = false;
	}
}

void ARTSPlayerController::ChangeBinding(int32 BindingIndex)
{
	if (!InputComponent) { return; }

	if (bRemovedBinding)
	{
		FInputActionBinding NewBinding = InputComponent->GetActionBinding(BindingIndex);
		FInputActionBinding OldBinding(RemovedMoveBinding.GetActionName(), RemovedMoveBinding.KeyEvent);
		OldBinding.ActionDelegate = NewBinding.ActionDelegate;
		InputComponent->AddActionBinding(OldBinding);
	}
}

void ARTSPlayerController::SetAidValue(bool bNewAidValue)
{
	bAidValue = bNewAidValue;
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
			//If character was selected but died in process 
			//or is inside building dont give him an order
			if (!Cast<ARTSPlayerUnit>(Actor)->IsCharacterDead()
				&& !Cast<ARTSPlayerUnit>(Actor)->IsCharacterInBuilding())
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
	this->AddBindingBack();
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
	this->AddBindingBack();
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
					if (this->WasInputKeyJustPressed(FKey(FName("H"))) || bAidValue)
					{
						Cast<ARTSAIController>(Actor->GetController())->Aid(Hit, EUnitState::Healing);
					}
					else if (this->WasInputKeyJustPressed(FKey(FName("C"))) || !bAidValue)
					{
						Cast<ARTSAIController>(Actor->GetController())->Aid(Hit, EUnitState::Cleansing);
					}
				}
			}
		}
	}
	this->AddBindingBack();
}