// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RTSprojGameMode.h"
#include "RTSprojPlayerController.h"
#include "RTSprojCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARTSprojGameMode::ARTSprojGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARTSprojPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}