// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// run on all the clients and server
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnMissionComplete(class APawn* InstigatorPawn, const bool bMissionSuccess);
};
