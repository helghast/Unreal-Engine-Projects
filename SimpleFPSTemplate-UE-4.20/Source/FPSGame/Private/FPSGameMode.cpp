// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "FPSGameState.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

	// use our gamestate class
	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(class APawn* InstigatorPawn, const bool bMissionSuccess)
{
	if (InstigatorPawn)
	{
		if (SpectatorViewpointClass)
		{
			TArray<AActor*> ReturnedActors;
			UGameplayStatics::GetAllActorsOfClass(this, SpectatorViewpointClass, ReturnedActors);

			// change viewtarget if any valid actor found
			if (ReturnedActors.Num() > 0)
			{
				AActor* NewViewTarget = ReturnedActors[0];

				for (FConstPlayerControllerIterator It(GetWorld()->GetPlayerControllerIterator()); It; ++It)
				{
					if (APlayerController* PC = It->Get())
					{
						PC->SetViewTargetWithBlend(NewViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("SpectatorViewpointClass is nullptr. Please update gamemode class with valid subclass"));
		}
	}

	if (AFPSGameState* GS = GetGameState<AFPSGameState>())
	{
		GS->MulticastOnMissionComplete(InstigatorPawn, bMissionSuccess);
	}
}
