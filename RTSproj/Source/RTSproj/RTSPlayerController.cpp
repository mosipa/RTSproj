// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerController.h"
#include "RTSHud.h"

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
}

void ARTSPlayerController::Select()
{
	UE_LOG(LogTemp, Warning, TEXT("Selecting"));
	HUDPtr->InitialPoint = HUDPtr->GetMousePosition2D();
	HUDPtr->bStartSelecting = true;
}

void ARTSPlayerController::FinishSelecting()
{
	UE_LOG(LogTemp, Warning, TEXT("FinishSelecting"));
	HUDPtr->bStartSelecting = false;
}

void ARTSPlayerController::Move()
{
	UE_LOG(LogTemp, Warning, TEXT("Moving"));
}