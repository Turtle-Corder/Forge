// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Forge.h"
#include "ForgeGameMode.h"
#include "ForgePlayerController.h"
#include "ForgeCharacter.h"

AForgeGameMode::AForgeGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AForgePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}