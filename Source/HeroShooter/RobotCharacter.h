// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RobotCharacter.generated.h"


class UCameraComponent;
class USkeletalMeshComponent;
class AWeapon;


UCLASS()
class HEROSHOOTER_API ARobotCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PlayerLegMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PlayerShadowMesh;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* RifleShadowMesh;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float FireRate;

	UPROPERTY(BlueprintReadWrite)
	bool bIsFiring;

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(VisibleDefaultsOnly)
	AWeapon* Weapon;

	void MoveForward(float AxisValue);
	void MoveSideways(float AxisValue);
	void LookUpGamepad(float AxisValue);
	void TurnSidewaysGamepad(float AxisValue);

	UPROPERTY(EditAnywhere)
	float GamepadTurnRateVertical = 75;

	UPROPERTY(EditAnywhere)
	float GamepadTurnRateHorizontal = 75;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	void StartFire();

	void StopFire();

	void FireShot();

	float TimeBetweenShots;

	FTimerHandle HandleReFire;


};
