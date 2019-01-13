// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard() 
	: GuardState(EAIState::Idle),
	bPatrol(false),
	CurrentPatrolPoint(nullptr),
	FirstPatrolPoint(nullptr),
	SecondPatrolPoint(nullptr)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// dont need attachment, because is not a scene component
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalRotation = GetActorRotation();

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

void AFPSAIGuard::OnPawnSeen(class APawn* SeenPawn)
{
	if (SeenPawn == nullptr)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.f, 12, FColor::Red, false, 10.f);

	if (AFPSGameMode* GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->CompleteMission(SeenPawn, false);
	}

	SetGuardState(EAIState::Alerted);

	// stop movement if patrolling
	if (AController* Controller = GetController())
	{
		Controller->StopMovement();
	}
}

void AFPSAIGuard::OnNoiseHeard(class APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (NoiseInstigator == nullptr && GuardState == EAIState::Alerted)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.f, 12, FColor::Green, false, 10.f);

	// UKismetMathLibrary::FindLookAtRotation
	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();

	// only want change the Yaw rotation
	NewLookAt.Pitch = NewLookAt.Roll = 0.f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.f);

	SetGuardState(EAIState::Suspicious);

	// stop movement if patrolling
	if (AController* Controller = GetController())
	{
		Controller->StopMovement();
	}
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	SetActorRotation(OriginalRotation);
	SetGuardState(EAIState::Idle);

	// stopped investigating...if we are a patrolling pawn, pick a new patrol point to move to
	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

void AFPSAIGuard::SetGuardState(const EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}

	GuardState = NewState;

	OnRep_GuardState(); // call for server too
}

void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// patrol goal checks
	if (CurrentPatrolPoint)
	{
		const FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		const float DistanceToGoal = Delta.Size();

		// check if we are within 75 units of our goal, if so - pick a new patrol point
		if (DistanceToGoal <= 75)
		{
			MoveToNextPatrolPoint();
		}
	}
}

void AFPSAIGuard::MoveToNextPatrolPoint()
{
	// only assign next patrol point
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
	{
		CurrentPatrolPoint = FirstPatrolPoint;
	}
	else
	{
		CurrentPatrolPoint = SecondPatrolPoint;
	}
	
	//UNavigationSystem is deprecated since 4.20. use UAIBlueprintHelperLibrary, add #include "Blueprint/AIBlueprintHelperLibrary.h" and add NavigationSystem module to CS
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}