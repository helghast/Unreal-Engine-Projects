// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"
#include "Gameframework/Pawn.h"
#include "FPSPlayerController.h"

void AFPSGameState::MulticastOnMissionComplete_Implementation(class APawn* InstigatorPawn, const bool bMissionSuccess)
{
	for (FConstPlayerControllerIterator It(GetWorld()->GetPlayerControllerIterator()); It; ++It)
	{
		AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			PC->OnMissionCompleted(InstigatorPawn, bMissionSuccess);

			if (APawn* MyPawn = PC->GetPawn())
			{
				MyPawn->DisableInput(PC);
			}
		}
	}
}