// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
	: PowerupInterval(0.f), 
	TotalNrOfTicks(0), 
	TicksProcessed(0),
	bIsPowerupActive(false)
{
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
	//ActivatePowerup();
}

void ASPowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	// last tick?
	if (TicksProcessed >= TotalNrOfTicks)
	{
		OnExpired();

		// replicate change to clients
		bIsPowerupActive = false;
		// and call OnRep_PowerupActive on SERVER too, to replicated to server as well
		OnRep_PowerupActive();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void ASPowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

void ASPowerupActor::ActivatePowerup(AActor* ActiveFor)
{
	OnActivated(ActiveFor);

	// replicate change to clients
	bIsPowerupActive = true;
	// and call OnRep_PowerupActive on SERVER too, to replicated to server as well
	OnRep_PowerupActive();

	if (PowerupInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
}

