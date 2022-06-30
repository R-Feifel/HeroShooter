// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotCharacter.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


// Sets default values
ARobotCharacter::ARobotCharacter()
{
		// Constructs the Camera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

		// Constructs the First Person Arms Mesh
	PlayerMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh1P"));
	PlayerMesh1P->SetupAttachment(FirstPersonCameraComponent);
	PlayerMesh1P->SetOnlyOwnerSee(true);
	PlayerMesh1P->bCastDynamicShadow = false;
	PlayerMesh1P->CastShadow = false;

	PlayerWeaponMesh =CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerRifleMesh"));
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

	//Initialize Movement Inputs
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ARobotCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveSideways"), this, &ARobotCharacter::MoveSideways);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("TurnSideways"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpGamepad"), this, &ARobotCharacter::LookUpGamepad);
	PlayerInputComponent->BindAxis(TEXT("TurnSidewaysGamepad"), this, &ARobotCharacter::TurnSidewaysGamepad);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	// Initialize Shooting Inputs
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ARobotCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ARobotCharacter::StopFire);

	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &ARobotCharacter::ReloadWeaponStart);
	/* 	Add input function for:
			Melee
			ADS
			Ability 1
			Ability 2
			Ultimate Ability
	 */
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

void ARobotCharacter::ReloadWeaponStart()
{
	if (BulletCount == MaxBulletCount) return;

	bIsFiring = false;
	bIsReloading = true;
	UE_LOG(LogTemp, Warning, TEXT("Reloading"));

	GetWorldTimerManager().SetTimer(HandleReloadDuration, this, &ARobotCharacter::ReloadWeaponFinished, ReloadDuration, false);
	GetWorldTimerManager().ClearTimer(HandleFiring);
}

void ARobotCharacter::ReloadWeaponFinished()
{
	BulletCount = MaxBulletCount;
	bIsReloading = false;	
	GetWorldTimerManager().ClearTimer(HandleReloadDuration);
	if (bIsFireHeld)
	{
		StartFire();
	}
}

void ARobotCharacter::StartFire()
{
	// bIsFireHeld = UGameplayStatics::GetPlayerController(GetWorld(), 0)->IsInputKeyDown(TEXT("LeftMouseButton"));
	if (BulletCount > 0 && !bIsReloading)
	{
		if(!HandleFireInput.IsValid()) 
		{
			FireShot();
		}

		if(bIsAutomatic)
		{
			GetWorldTimerManager().SetTimer(HandleFiring, this, &ARobotCharacter::FireShot, TimeBetweenShots, true);
		}
	} else
	{
		bIsFireHeld = true;
	}
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

	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, PlayerWeaponMesh, TEXT("MuzzleFlash"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, PlayerWeaponMesh, TEXT("MuzzleFlash"));

	FHitResult Hit;
	FVector ShotDirection;
	bool bSuccess = GunTrace(Hit, ShotDirection);
	if (bSuccess)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
		AActor* HitActor = Hit.GetActor();
		if (HitActor != nullptr)
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			AController* RBCController = GetController();
			if(RBCController != nullptr)
			{
				HitActor->TakeDamage(Damage, DamageEvent, RBCController, this);
			}
		}

	}
	BulletCount--;
}

bool ARobotCharacter::GunTrace(FHitResult& Hit, FVector& ShotDirection)
{
	AController* RBCController = GetController();
	if(RBCController == nullptr) return false;	

	FVector Location;
	FRotator Rotation;

	RBCController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = -Rotation.Vector();

	FVector End = Location + Rotation.Vector() * MaxRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor((this));
	Params.AddIgnoredActor(GetOwner());
	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
}

