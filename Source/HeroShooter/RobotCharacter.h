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

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Rate of Fire in RPM (Rounds per Minute)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	float FireRate;

	UPROPERTY(BlueprintReadWrite)
	bool bIsFiring;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = Weapon)
	bool bIsAutomatic = true;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	uint8 MaxBulletCount = 30;

	UPROPERTY(BlueprintReadOnly, Category = Weapon)
	uint8 BulletCount;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float Damage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float MaxRange = 10000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	float ReloadDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
	USoundBase* MuzzleSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
	USoundBase* ImpactSound;



private:

	UPROPERTY(EditDefaultsOnly, Category = Player)
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere)
	float Health;

	void MoveForward(float AxisValue);
	void MoveSideways(float AxisValue);
	void LookUpGamepad(float AxisValue);
	void TurnSidewaysGamepad(float AxisValue);
	void ReloadWeaponStart();
	void ReloadWeaponFinished();

	bool bIsReloading = false;
	bool bIsFireHeld = false;


	FTimerHandle HandleReloadDuration;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float GamepadTurnRateVertical = 75;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float GamepadTurnRateHorizontal = 75;

	void StartFire();
	void StopFire();
	void ReenableWeapon();
	void FireShot();

	float TimeBetweenShots;


	FTimerHandle HandleFiring;
	FTimerHandle HandleFireInput;

	bool GunTrace(FHitResult& Hit, FVector& ShotDirection);


};
