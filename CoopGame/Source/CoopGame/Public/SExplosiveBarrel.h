// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

	/** Returns the properties used for network replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Components)
	class USHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class URadialForceComponent* RadialForceComp;

	UFUNCTION()
	void OnHealthChanged(class USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser);

	/* impulse applied to the barrel mesh when it explodes to boost it up a little */
	UPROPERTY(EditDefaultsOnly, Category = FX)
	float ExplosionImpulse;

	/* particle to play when health reached to zero */
	UPROPERTY(EditDefaultsOnly, Category = FX)
	class UParticleSystem* ExplosionEffect;

	/* the material to replace the original on the mesh once exploded (a blackened version) */
	UPROPERTY(EditDefaultsOnly, Category = FX)
	class UMaterialInterface* ExplosionMaterial;

	/* move here no gameplay code significance */
	UFUNCTION()
	void OnRep_Exploded();

private:
	UPROPERTY(ReplicatedUsing = OnRep_Exploded)
	bool bExploded;
};
