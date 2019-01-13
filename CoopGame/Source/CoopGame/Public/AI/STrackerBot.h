// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	class USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	class USphereComponent* SphereComp;

	UFUNCTION()
	void HandleTakeDamage(class USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser);

	FVector GetNextPathPoint();

	void RefreshPath();

	// next point in navigation path
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = TrackerBot)
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = TrackerBot)
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = TrackerBot)
	float RequiredDistanceToTarget;

	// dynamic material to pulse on damage
	class UMaterialInstanceDynamic* MatInst;

	UFUNCTION()
	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = TrackerBot)
	class UParticleSystem* ExplosionEffect;

	bool bExploded;

	bool bStartSelfDestruction;

	UPROPERTY(EditDefaultsOnly, Category = TrackerBot)
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = TrackerBot)
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = TrackerBot)
	float SelfDamageInterval;

	FTimerHandle TimerHandle_SelfDamage;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	class USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	class USoundCue* ExplodeSound;

	// challenge code
	// the power boost of the bot, affects damaged caused to enemies and color of the bot. range 1 to 4
	int32 PowerLevel;

	// find nearby enemies and grow in powerlevel based on the amount
	UFUNCTION()
	void OnCheckNearbyBots();
	// end challenge code

	FTimerHandle TimerHandle_RefreshPath;

public:	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
