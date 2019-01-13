// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	WaveInProgress,
	// no longer spawning new bots, waiting for players to kill remaining bots
	WaitingToComplete,
	WaveComplete,
	GameOver
};

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	void OnRep_WaveState(const EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = GameSate)
	void WaveStateChanged(const EWaveState NewState, const EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_WaveState, Category = GameState)
	EWaveState WaveState;

public:
	/** Returns the properties used for network replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetWaveState(const EWaveState NewState);
};
