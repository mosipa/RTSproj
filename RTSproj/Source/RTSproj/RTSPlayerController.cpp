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
#include "Building.h"
#include "Prison.h"
#include "PlayersHideout.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "MyMathClass.h"
#include "SewerEntrance.h"

ARTSPlayerController::ARTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;

	bRemovedBinding = false;
	bAidValue = false;
	bDisableInputs = false;

	HideoutPtr = nullptr;
	SewerPtr = nullptr;

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
	//At least to the moment I find a better way 
	RemovedMoveBinding = InputComponent->GetActionBinding(LEFTMOUSEBUTTONACTION_ID);

	//TODO create widgets here but add to playerscreen when player chooses unit
	//As there are building which shouldn't have move/fire buttons
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
	if (!HUDPtr || bDisableInputs) { return; }

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
	InputComponent->BindAction("ReleaseUnits", IE_Pressed, this, &ARTSPlayerController::ReleaseUnits);
	InputComponent->BindAction("GetBuildingCamera", IE_Pressed, this, &ARTSPlayerController::GetBuildingCamera);
	InputComponent->BindAction("ToggleTransparency", IE_Pressed, this, &ARTSPlayerController::ToggleTransparency); 
	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ARTSPlayerController::LeftMouseButtonActions);
}

void ARTSPlayerController::RemoveMoveBinding()
{
	if (!InputComponent) { return; }

	//In case it's been already removed - dont do it again
	if (!bRemovedBinding)
	{
		//11 - LeftMouseButtonActions - LEFTMOUSEBUTTONACTION_ID
		//TODO find a way to get FInputActionBinding with a NAME, not int
		//use BindingToRemove.GetActionName().ToString() to get name, maybe there's something in it
		//compare to all bindings and find the one (MOVE) to remove
		InputComponent->RemoveActionBinding(LEFTMOUSEBUTTONACTION_ID);
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
		//TODO fix
		//Adding this line prevents from adding additional useless bindings
		//But causes user to being unable using User_HUD Move button (input)
		InputComponent->RemoveActionBinding(LEFTMOUSEBUTTONACTION_ID);
		
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

APlayersHideout* ARTSPlayerController::GetSelectedBuilding()
{
	return HideoutPtr;
}

void ARTSPlayerController::ZoomIn()
{
	if (bDisableInputs) { return; }

	FVector CameraLocation = this->GetPawn()->GetActorLocation();
	CameraLocation.Z -= 20.f;
	this->GetPawn()->SetActorLocation(CameraLocation);
}

void ARTSPlayerController::ZoomOut()
{
	if (bDisableInputs) { return; }

	FVector CameraLocation = this->GetPawn()->GetActorLocation();
	CameraLocation.Z += 20.f;
	this->GetPawn()->SetActorLocation(CameraLocation);
}

void ARTSPlayerController::Select()
{
	if (!HUDPtr) { return; }
	HUDPtr->InitialPoint = HUDPtr->GetMousePosition2D();
	HUDPtr->bStartSelecting = true;

	HideoutPtr = nullptr;
}

void ARTSPlayerController::FinishSelecting()
{
	if (!HUDPtr) { return; }
	HUDPtr->bStartSelecting = false;
}

void ARTSPlayerController::LeftMouseButtonActions()
{
	if (!HUDPtr || bDisableInputs) { return; }
	
	FHitResult Hit;
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit))
	{
		//If there are any units under selection
		if (HUDPtr->GetSelectedActors().Num() > 0)
		{
			HideoutPtr = nullptr;

			TArray<ARTSPlayerUnit*> SelectedActors = HUDPtr->GetSelectedActors();

			for (auto Actor : SelectedActors)
			{
				//If character was selected but died in process 
				//or is inside building dont give him an order
				if (!Cast<ARTSPlayerUnit>(Actor)->IsCharacterDead()
					&& !Cast<ARTSPlayerUnit>(Actor)->IsCharacterInBuilding())
				{
					//If units are under selection
					//And we point at player's hideout
					if (Hit.GetActor()->GetClass()->IsChildOf<APlayersHideout>())
					{
						APlayersHideout* Building = Cast<APlayersHideout>(Hit.GetActor());
						Cast<ARTSAIController>(Actor->GetController())->GetCloserToBuilding(Building, EUnitState::Entering);
					}
					//If units are under selection
					//And we point at prison
					else if (Hit.GetActor()->GetClass()->IsChildOf<APrison>())
					{
						APrison* Prison = Cast<APrison>(Hit.GetActor());
						Cast<ARTSAIController>(Actor->GetController())->GetCloserToBuilding(Prison, EUnitState::Releasing);
					}
					//If units are under selection
					//And we point at sewer entrance
					else if (Hit.GetActor()->GetClass()->IsChildOf<ASewerEntrance>())
					{
						UE_LOG(LogTemp, Warning, TEXT("Ordering units to move towards sewer's entrance"));
						//TODO add logic + decide if entering sewer by unit should change level
					}
					//Else move to pointed location
					else
					{
						FVector MoveTo = Hit.Location;
						Cast<ARTSAIController>(Actor->GetController())->Move(MoveTo);
					}
				}
			}
		//If there aren't any units under selection
		//And we point at building
		}
		else if (Hit.GetActor()->GetClass()->IsChildOf<APlayersHideout>())
		{
			HideoutPtr = Cast<APlayersHideout>(Hit.GetActor());
			UE_LOG(LogTemp, Warning, TEXT("Building %s seleceted"), *(HideoutPtr->GetName()));
		}
		//If there aren't any units under selection
		//And we point at sewer entrance
		else if (Hit.GetActor()->GetClass()->IsChildOf<ASewerEntrance>())
		{
			SewerPtr = Cast<ASewerEntrance>(Hit.GetActor());
			UE_LOG(LogTemp, Warning, TEXT("Opening sewer level"));
		}
	}
}

void ARTSPlayerController::ReleaseUnits()
{
	if (HideoutPtr && !bDisableInputs)
	{
		UE_LOG(LogTemp, Warning, TEXT("RELEASING"));
		HideoutPtr->ReleaseUnits();
	}
}

//TESTING
//Want to add input action - seeing through units eyes (scouts)
//Testing it on buildings
void ARTSPlayerController::GetBuildingCamera()
{	
	//If building is selected get view
	if (HideoutPtr)
	{
		//Block all inputs that needed to be blocked and save player's camera location and rotation
		if (!bDisableInputs)
		{
			bDisableInputs = true;
			PrevCamLocation = this->GetPawn()->GetRootComponent()->GetComponentLocation();
			PrevCamRotation = this->GetPawn()->GetRootComponent()->GetComponentRotation();
		}

		UE_LOG(LogTemp, Warning, TEXT("Getting view of building"));
		FVector CamLoc = HideoutPtr->GetPossibleCamLocations()[CurrentIndex] + HideoutPtr->GetRootComponent()->GetComponentLocation();
		FRotator CamRot = HideoutPtr->GetPossibleCamRotations()[CurrentIndex];

		//This needs to be set in order to MoveComponentTo to work
		//At least CallbackTarget has to be set
		FLatentActionInfo info = FLatentActionInfo();
		info.CallbackTarget = this->GetPawn();

		//Moving camera
		UKismetSystemLibrary::MoveComponentTo(this->GetPawn()->GetRootComponent(), CamLoc, CamRot, true, false, 0.2f, true, EMoveComponentAction::Move, info);

		CurrentIndex = (CurrentIndex + 1) % HideoutPtr->GetPossibleCamLocations().Num();
	}
	//Get back to camera starting position
	else
	{
		bDisableInputs = false;

		this->GetPawn()->GetRootComponent()->SetWorldLocationAndRotation(PrevCamLocation, PrevCamRotation);
		CurrentIndex = 0;
	}
}

void ARTSPlayerController::ToggleTransparency()
{
	if (!HUDPtr || HUDPtr->GetSelectedActors().Num() < 1 || bDisableInputs) { return; }

	//Only one unit to tell the distance
	ARTSPlayerUnit* PlayerUnit = Cast<ARTSPlayerUnit>(HUDPtr->GetSelectedActors()[0]);

	if (!PlayerUnit) { return; }

	FHitResult Hit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, Hit);

	APlayersHideout* PlayersHideout = Cast<APlayersHideout>(Hit.GetActor());

	//Check if player's pointing at APlayerHideout 
	//IMPORTANT TODO for starter we're going to try do it with this type of buildings - change to work for all needed object
	if (PlayersHideout)
	{
		float Distance = MyMathClass::GetDistance(PlayerUnit->GetActorLocation(), PlayersHideout->GetActorLocation());

		//TODO Figure out the best distance to activate it
		if (Distance <= 400.f)
		{
			//Change material - it's going to be decide inside building class if object is going to be transparent or not (depend on previous state of building's material)
			PlayersHideout->ToggleTransparency();
		}
	}
}

void ARTSPlayerController::Knife()
{
	if (!HUDPtr || bDisableInputs) { return; }
	
	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSPlayerUnit*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit))
		{
			for (auto Actor : SelectedActors)
			{
				//If character was selected but died in process dont give him an order
				if (!Cast<ARTSPlayerUnit>(Actor)->IsCharacterDead()
					&& !Cast<ARTSPlayerUnit>(Actor)->IsCharacterInBuilding())
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
	if (!HUDPtr || bDisableInputs) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSPlayerUnit*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit))
		{
			for (auto Actor : SelectedActors)
			{
				//If character was selected but died in process dont give him an order
				if (!Cast<ARTSPlayerUnit>(Actor)->IsCharacterDead()
					&& !Cast<ARTSPlayerUnit>(Actor)->IsCharacterInBuilding())
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
	if (!HUDPtr || bDisableInputs) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSPlayerUnit*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit))
		{
			for (auto Actor : SelectedActors)
			{
				if (!Cast<ARTSPlayerUnit>(Actor)->IsCharacterDead()
					&& !Cast<ARTSPlayerUnit>(Actor)->IsCharacterInBuilding())
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