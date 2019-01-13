// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"
#include "Net/UnrealNetwork.h"

void ASGameState::OnRep_WaveState(const EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, WaveState);
}

void ASGameState::SetWaveState(const EWaveState NewState)
{
	if (Role == ROLE_Authority)
	{
		const EWaveState OldState = WaveState;

		// replicate change to clients
		WaveState = NewState;

		// call on server too
		OnRep_WaveState(OldState);
	}
}
