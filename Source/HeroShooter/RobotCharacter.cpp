// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotCharacter.h"
#include "Weapon.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
ARobotCharacter::ARobotCharacter()
{

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	PlayerShadowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerShadowMesh"));
	PlayerShadowMesh->SetupAttachment(GetCapsuleComponent());

	PlayerLegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerLegMesh"));
	PlayerLegMesh->SetupAttachment(GetCapsuleComponent());
	PlayerLegMesh->SetOnlyOwnerSee(true);
	PlayerLegMesh->bCastDynamicShadow = false;
	PlayerLegMesh->CastShadow = false;

	RifleShadowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleShadowMesh"));

}

// Called when the game starts or when spawned
void ARobotCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;

	TimeBetweenShots = 1 / (FireRate / 60);

	Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	Weapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::KeepRelativeTransform, TEXT("GripPoint"));
	Weapon->SetOwner(this);

	PlayerLegMesh->HideBoneByName(TEXT("spine_01"), EPhysBodyOp::PBO_None);
}

float ARobotCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called to bind functionality to input
void ARobotCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ARobotCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveSideways"), this, &ARobotCharacter::MoveSideways);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("TurnSideways"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpGamepad"), this, &ARobotCharacter::LookUpGamepad);
	PlayerInputComponent->BindAxis(TEXT("TurnSidewaysGamepad"), this, &ARobotCharacter::TurnSidewaysGamepad);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);


	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ARobotCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ARobotCharacter::StopFire);

}

void ARobotCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue);	
}

void ARobotCharacter::MoveSideways(float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue);
}

void ARobotCharacter::LookUpGamepad(float AxisValue)
{	
	AddControllerPitchInput(AxisValue * GamepadTurnRateVertical * GetWorld()->GetDeltaSeconds());
}

void ARobotCharacter::TurnSidewaysGamepad(float AxisValue)
{
	AddControllerYawInput(AxisValue * GamepadTurnRateHorizontal * GetWorld()->GetDeltaSeconds());
}

void ARobotCharacter::StartFire()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Start Fire"));

	bIsFiring = true;
	FireShot();
	GetWorldTimerManager().SetTimer(HandleReFire, this, &ARobotCharacter::FireShot, TimeBetweenShots, true);
}

void ARobotCharacter::StopFire()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("End Fire"));

	GetWorldTimerManager().ClearTimer(HandleReFire);
	bIsFiring = false;
}

void ARobotCharacter::FireShot()
{
	Weapon->PullTrigger();
}


