// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// time between power up ticks
	UPROPERTY(EditDefaultsOnly, Category = Powerups)
	float PowerupInterval;

	// total times we apply the power up effect
	UPROPERTY(EditDefaultsOnly, Category = Powerups)
	int32 TotalNrOfTicks;

	FTimerHandle TimerHandle_PowerupTick;

	UFUNCTION()
	void OnTickPowerup();

	// keeps state of the power up
	UPROPERTY(ReplicatedUsing = OnRep_PowerupActive)
	bool bIsPowerupActive;

	UFUNCTION()
	void OnRep_PowerupActive();

	UFUNCTION(BlueprintImplementableEvent, Category = Powerups)
	void OnPowerupStateChanged(const bool bNewIsActive);

public:	
	void ActivatePowerup(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = Powerups)
	void OnActivated(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = Powerups)
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = Powerups)
	void OnExpired();

	/** Returns the properties used for network replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// total number of ticks applied
	int32 TicksProcessed;
};
