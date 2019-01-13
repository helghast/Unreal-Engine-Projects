// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

// declare custom event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, class USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, class AActor*, DamageCauser);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

	/** Returns the properties used for network replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = HealthComponent)
	void Heal(const float HealAmount);

	bool bIsDead;

	// only Replicated this if member can change between team when respawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HealthComponent)
	uint8 TeamNum;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = HealthComponent)
	float Health;

	UFUNCTION()
	void OnRep_Health(const float /* is the previous replicated health value */OldHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealthComponent)
	float DefaultHealth;

	UFUNCTION()
	void HandleTakeAnyDamage(class AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser);

public:
	float GetHealth() const
	{
		return Health;
	}

	UFUNCTION(BlueprintPure, Category = HealthComponent)
	static bool IsFriendly(class AActor* A, class AActor* B);
};
