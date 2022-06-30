// Copyright Epic Games, Inc. All Rights Reserved.

#include "HeroShooterGameMode.h"
#include "HeroShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHeroShooterGameMode::AHeroShooterGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/ContentPacks/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
