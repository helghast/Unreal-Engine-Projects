// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, class AActor*, VictimActor, class AActor*, KillerActor, class AController*, KillerController);

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASGameMode();

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, Category = GameMode)
	FOnActorKilled OnActorKilled;

protected:	
	// hook for bp to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = GameMode)
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	// start spawning bots
	void StartWave();
	
	// stop spawning bots
	void EndWave();

	// set timer for next startwave
	void PrepareForNextWave();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void RestartDeadPlayer();

	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHanlde_NextWaveStart;

	// bots to spawn in current wave
	int32 NrOfBotsToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	bool bAllowRestartPlayer;

	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	float TimeBetweenWaves;

	void SetWaveState(const EWaveState NewState);
};
