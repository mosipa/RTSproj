// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RTSprojGameMode.h"
#include "RTSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "RTSPlayerController.h"

ARTSprojGameMode::ARTSprojGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARTSPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}