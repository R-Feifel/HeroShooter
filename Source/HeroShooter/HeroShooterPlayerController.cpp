// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroShooterPlayerController.h"
#include "Blueprint/UserWidget.h"


void AHeroShooterPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UUserWidget *HUD = CreateWidget(this, HUDClass);
    if (HUD != nullptr)
    { 
        HUD->AddToViewport();
    }
}
