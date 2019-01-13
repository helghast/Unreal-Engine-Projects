// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Suspicious,
	Alerted
};

// need to add AIModule to CS
UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(class APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(class APawn* NoiseInstigator, const FVector& Location, float Volume);

	UFUNCTION()
	void ResetOrientation();

	void SetGuardState(const EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = AI)
	void OnStateChanged(const EAIState NewState);

	// only replicated on clients
	UFUNCTION()
	void OnRep_GuardState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FRotator OriginalRotation;
	FTimerHandle TimerHandle_ResetOrientation;

	UPROPERTY(ReplicatedUsing = OnRep_GuardState)
	EAIState GuardState;

protected:
	// CHALLENGE CODE

	// let the guard go on patrol
	UPROPERTY(EditInstanceOnly, Category = AI)
	bool bPatrol;

	// first of two patrol points to patrol between
	UPROPERTY(EditInstanceOnly, Category = AI, meta = (EditCondition = "bPatrol"))
	AActor* FirstPatrolPoint;

	// second of two patrol points to patrol between
	UPROPERTY(EditInstanceOnly, Category = AI, meta = (EditCondition = "bPatrol"))
	AActor* SecondPatrolPoint;

	// the current point the actor is either moving to or standing at
	AActor* CurrentPatrolPoint;

	void MoveToNextPatrolPoint();
};
