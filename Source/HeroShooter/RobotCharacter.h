// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RobotCharacter.generated.h"


class UCameraComponent;
class USkeletalMeshComponent;


UCLASS()
class HEROSHOOTER_API ARobotCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PlayerMesh1P;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PlayerWeaponMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PlayerLegMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShadowPlayerMesh;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShadowWeaponMesh;

public:
	// Sets default values for this character's properties
	ARobotCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	UPROPERTY(BlueprintReadWrite)
	bool bIsCrouching = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSprinting = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsWalking = true;

	UPROPERTY(BlueprintReadOnly)
	bool bCanFire = true;

	UPROPERTY(BlueprintReadWrite)
	bool bIsFiring = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsReloading = false;

	UPROPERTY(BlueprintReadOnly)
	bool bCanAim = true;

	UPROPERTY(BlueprintReadWrite)
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsAimHeld = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsReturningToHipfire = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	bool bIsAutomatic = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float WalkSpeed = 600;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float SprintSpeed = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float CrouchedSpeed = 300;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Player)
	float FOV = 90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Player)
	float SprintFOVScalar = 1.4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Player)
	float ADSFOVScalar = .8;

	UPROPERTY(EditDefaultsOnly, Category = Player)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere, Category = Player)
	float Health;

	UPROPERTY(EditAnywhere, Category = Player)
	float MeleeDamage = 50.f;

	// Rate of Fire in RPM (Rounds per Minute)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	uint8 MaxBulletCount = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	uint8 BulletCount;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float WeaponDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	float ADSSpeedScalar = .15f;	

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float MaxRange = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float ADSZoomDistance = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	float ReloadDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float ShotSpread = 350.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
	USoundBase* MuzzleSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
	USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
	USoundBase* EmptyClickSound;



private:

	void MoveForward(float AxisValue);
	void MoveSideways(float AxisValue);
	void LookUpGamepad(float AxisValue);
	void TurnSidewaysGamepad(float AxisValue);

	void JumpPressed();
	void CrouchPressed();
	void BeginCrouch();
	void StandUp();
	void BeginSprint();
	void EndSprint();

	void ReloadWeaponStart();
	void ReloadWeaponFinished();
	void AimDownSights();
	void ReturnToHipFire();

	bool bIsFireHeld = false;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float GamepadTurnRateVertical = 75;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float GamepadTurnRateHorizontal = 75;

	float CapsHeight;

	FVector CameraLocation;
	FRotator CameraRotation;
	FVector AimSocketLocation;
	FRotator AimSocketRotation;
	FVector ADSLocation;
	FRotator ADSRotation;

	void StartFire();
	void StopFire();
	void ReenableWeapon();
	void FireShot();

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float TimeBetweenShots;

	FTimerHandle HandleReloadDuration;
	FTimerHandle HandleReloadSoundDelay;
	FTimerHandle HandleFiring;
	FTimerHandle HandleFireInput;

	bool GunTrace(FHitResult& Hit, FVector& ShotDirection);

};
