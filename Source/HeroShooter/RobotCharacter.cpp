// Fill out your copyright notice in the Description page of Project Settings.

#include "RobotCharacter.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h"

// Sets default values
ARobotCharacter::ARobotCharacter()
{
	// Constructs the Camera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetFieldOfView(FOV);

	// Constructs the First Person Arms Mesh
	PlayerMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh1P"));
	PlayerMesh1P->SetupAttachment(FirstPersonCameraComponent);
	PlayerMesh1P->SetOnlyOwnerSee(true);
	PlayerMesh1P->bCastDynamicShadow = false;
	PlayerMesh1P->CastShadow = false;

	// Constructs the First Person Weapon Mesh
	PlayerWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerRifleMesh"));
	PlayerWeaponMesh->SetOnlyOwnerSee(true);
	PlayerWeaponMesh->bCastDynamicShadow = false;
	PlayerWeaponMesh->CastShadow = false;

	// Constructs the Leg Mesh that the player sees when they look down
	PlayerLegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerLegMesh"));
	PlayerLegMesh->SetupAttachment(GetCapsuleComponent());
	PlayerLegMesh->SetOnlyOwnerSee(true);
	PlayerLegMesh->bCastDynamicShadow = false;
	PlayerLegMesh->CastShadow = false;

	// Constructs the Mesh that creates the player's shadow
	ShadowPlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerShadowMesh"));
	ShadowPlayerMesh->SetupAttachment(GetCapsuleComponent());
	ShadowPlayerMesh->bRenderInMainPass = false;

	// Constructs the Mesh that creates the player's weapon's shadow
	ShadowWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShadowWeaponMesh"));
	ShadowWeaponMesh->bRenderInMainPass = false;
}

// Called when the game starts or when spawned
void ARobotCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	BulletCount = MaxBulletCount;

	TimeBetweenShots = FireRate / 60;
	TimeBetweenShots = 1 / TimeBetweenShots;

	PlayerWeaponMesh->AttachToComponent(PlayerMesh1P, FAttachmentTransformRules::KeepRelativeTransform, TEXT("GripPoint"));

	PlayerLegMesh->HideBoneByName(TEXT("spine_02"), EPhysBodyOp::PBO_None);

	CapsHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void ARobotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ARobotCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called to bind functionality to input
void ARobotCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Initialize Movement Inputs
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ARobotCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveSideways"), this, &ARobotCharacter::MoveSideways);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("TurnSideways"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpGamepad"), this, &ARobotCharacter::LookUpGamepad);
	PlayerInputComponent->BindAxis(TEXT("TurnSidewaysGamepad"), this, &ARobotCharacter::TurnSidewaysGamepad);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ARobotCharacter::JumpPressed);

	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ARobotCharacter::CrouchPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &ARobotCharacter::BeginSprint);

	// Initialize Shooting Inputs
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &ARobotCharacter::ReloadWeaponStart);

	PlayerInputComponent->BindAction(TEXT("ADS"), EInputEvent::IE_Pressed, this, &ARobotCharacter::AimDownSights);
	PlayerInputComponent->BindAction(TEXT("ADS"), EInputEvent::IE_Released, this, &ARobotCharacter::ReturnToHipFire);

	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ARobotCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ARobotCharacter::StopFire);

	/* 	TODO:

		Add input function for:
			Melee
			Ability 1
			Ability 2
			Ultimate Ability
	 */
}

void ARobotCharacter::MoveForward(float AxisValue)
{
	if ((bIsSprinting && AxisValue >= 0) || !bIsSprinting)
	{
		AddMovementInput(GetActorForwardVector(), AxisValue);
	}
	if (bIsSprinting && AxisValue <= 0)
	{
		EndSprint();
	}
}
void ARobotCharacter::MoveSideways(float AxisValue)
{
	if (bIsSprinting)
	{
		AxisValue = AxisValue * 0.1f;
	}
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

void ARobotCharacter::JumpPressed()
{
	Jump();
	if(bIsCrouching)
	{
		CrouchPressed();
	}
	// if(bIsSprinting)
	// {
	// 	EndSprint();
	// }
}

void ARobotCharacter::CrouchPressed()
{
	if (bIsCrouching)
	{
		bIsCrouching = false;
	}
	else if (!bIsCrouching)
	{
		bIsCrouching = true;
	}

	if (bIsCrouching)
	{
		BeginCrouch();
	}
	else if (!bIsCrouching)
	{
		StandUp();
	}
}

void ARobotCharacter::BeginCrouch()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = CrouchedSpeed;
}

void ARobotCharacter::StandUp()
{
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ARobotCharacter::BeginSprint()
{
	bIsSprinting = true;
	bIsWalking = false;
	if(bIsAiming)
	{
		ReturnToHipFire();
	}
	if (bIsCrouching)
	{
		CrouchPressed();
	}
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ARobotCharacter::EndSprint()
{
	bIsWalking = true;
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ARobotCharacter::ReloadWeaponStart()
{
	if (BulletCount == MaxBulletCount || bIsReloading == true)
		return;

	if (bIsAiming)
	{
		ReturnToHipFire();
		bIsAimHeld = true;
	}
		if (bIsSprinting)
	{
		EndSprint();
	}
	bIsFiring = false;
	bIsReloading = true;

	GetWorldTimerManager().SetTimer(HandleReloadDuration, this, &ARobotCharacter::ReloadWeaponFinished, ReloadDuration, false);
	GetWorldTimerManager().ClearTimer(HandleFiring);
}

void ARobotCharacter::ReloadWeaponFinished()
{
	bIsReloading = false;

	GetWorldTimerManager().ClearTimer(HandleReloadDuration);

	if (bIsFireHeld)
	{
		StartFire();
	}

	if (bIsAimHeld)
	{
		AimDownSights();
	}
}

void ARobotCharacter::AimDownSights()
{
	bIsAimHeld = true;
	if (bIsSprinting)
	{
		EndSprint();
	}
	
	if (!bIsReloading)
	{
		bIsAiming = true;
		bIsReturningToHipfire = false;
	}
}

void ARobotCharacter::ReturnToHipFire()
{
	if (bIsAiming)
	{
		bIsAiming = false;
		bIsReturningToHipfire = true;
	}
	bIsAimHeld = false;
}

void ARobotCharacter::StartFire()
{
	if (BulletCount > 0 && !bIsReloading && bCanFire)
	{
		if(bIsSprinting)
		{
			EndSprint();
		}
		if (!HandleFireInput.IsValid())
		{
			FireShot();
		}

		if (bIsAutomatic)
		{
			GetWorldTimerManager().SetTimer(HandleFiring, this, &ARobotCharacter::FireShot, TimeBetweenShots, true);
		}
	}
	else if (BulletCount == 0)
	{
		UGameplayStatics::SpawnSoundAttached(EmptyClickSound, PlayerWeaponMesh, TEXT("AmmoEject"));
	}
	bIsFireHeld = true;
}

void ARobotCharacter::StopFire()
{
	if (bIsFiring || BulletCount == 0)
	{
		GetWorldTimerManager().SetTimer(HandleFireInput, this, &ARobotCharacter::ReenableWeapon, TimeBetweenShots, true);
	}
	GetWorldTimerManager().ClearTimer(HandleFiring);

	bIsFiring = false;
	bIsFireHeld = false;
}

void ARobotCharacter::ReenableWeapon()
{
	GetWorldTimerManager().ClearTimer(HandleFireInput);
}

void ARobotCharacter::FireShot()
{
	if (BulletCount == 0)
	{
		GetWorldTimerManager().ClearTimer(HandleFiring);
		bIsFiring = false;
		return;
	}

	bIsFiring = true;

	if(bIsAiming)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, PlayerWeaponMesh, TEXT("MuzzleFlash"), FVector(0.f), FRotator(0.f), FVector(0.4f));
	} else
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, PlayerWeaponMesh, TEXT("MuzzleFlash"));
	}

	UGameplayStatics::SpawnSoundAttached(MuzzleSound, PlayerWeaponMesh, TEXT("MuzzleFlash"));

	FHitResult Hit;
	FVector ShotDirection;
	FCollisionQueryParams Params;
	bool bSuccess = GunTrace(Hit, ShotDirection);
	if (bSuccess)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
		AActor *HitActor = Hit.GetActor();
		if (HitActor != nullptr)
		{
			FPointDamageEvent DamageEvent(WeaponDamage, Hit, ShotDirection, nullptr);
			AController *RBCController = GetController();
			if (RBCController != nullptr)
			{
				HitActor->TakeDamage(WeaponDamage, DamageEvent, RBCController, this);
			}
		}
	}
	if (!bIsAutomatic)
	{
		StopFire();
	}
	BulletCount--;
}

bool ARobotCharacter::GunTrace(FHitResult &Hit, FVector &ShotDirection)
{
	AController *RBCController = GetController();
	if (RBCController == nullptr)
		return false;

	FVector Location;
	FRotator Rotation;
	FVector End;

	FVector Spread;
	Spread.X = FMath::RandRange(ShotSpread, -ShotSpread);
	Spread.Y = FMath::RandRange(ShotSpread, -ShotSpread);
	Spread.Z = FMath::RandRange(ShotSpread, -ShotSpread);
	// UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), Spread.X, Spread.Y, Spread.Z);

	RBCController->GetPlayerViewPoint(Location, Rotation);

	ShotDirection = -Rotation.Vector();

	if (bIsAiming)
	{
		End = Location + Rotation.Vector() * MaxRange;
	}
	else
	{
		End = (Location + Spread) + Rotation.Vector() * MaxRange;
	}
	DrawDebugLine(GetWorld(), Location, End, FColor::Red, false, 5.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
}
