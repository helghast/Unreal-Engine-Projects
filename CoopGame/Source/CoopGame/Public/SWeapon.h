// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

/* contains information of a single hitscane weapon linetrace */
USTRUCT()	
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	void StartFire();
	void StopFire();

	/** Returns the properties used for network replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	class USkeletalMeshComponent* MeshComp;

	virtual void PlayFireEffects(const FVector& TracerEndPoint);

	void PlayCameraShake();

	void PlayImpactEffects(const EPhysicalSurface SurfaceType, const FVector& ImpactPoint);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	class UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	class UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	class UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<class UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float BaseDamage;

	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	/* amount of bullets per minute. A Value = 60, is one shot per second, and a value = 600 are 10 shots per second or automatic fire */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float RateOfFire;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

	// bullet spread in degrees
	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (ClampMin=0.0f))
	float BulletSpread;

private:
	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastFireTime;

	/* derived of RateOfFire */
	float TimeBetweenShots;
};
