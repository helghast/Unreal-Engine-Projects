// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	class USHealthComponent* HealthComp;

	bool bWantsToZoom;
	
	/* Default FOV set during begin play */
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = Player)
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = Player, meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	void BeginZoom();
	void EndZoom();

	UPROPERTY(EditDefaultsOnly, Category = Player)
	TSubclassOf<class ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = Player)
	FName WeaponAttachSocketName;

	UFUNCTION()
	void OnHealthChanged(class USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser);

	/* pawn died previously */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Player)
	bool bDied;

	UPROPERTY(EditDefaultsOnly, Category = Player)
	float DestroyLifeSpan;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Player)
	class ASWeapon* CurrentWeapon;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** @return	Pawn's eye location */
	FVector GetPawnViewLocation() const override;

	/** Returns the properties used for network replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = Player)
	void StartFire();
	
	UFUNCTION(BlueprintCallable, Category = Player)
	void StopFire();
};
