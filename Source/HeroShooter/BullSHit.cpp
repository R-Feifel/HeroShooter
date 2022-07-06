// Fill out your copyright notice in the Description page of Project Settings.


#include "BullSHit.h"

// Sets default values
ABullSHit::ABullSHit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABullSHit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullSHit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABullSHit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

