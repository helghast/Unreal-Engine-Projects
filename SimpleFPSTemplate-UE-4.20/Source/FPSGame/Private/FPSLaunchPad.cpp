// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSLaunchPad.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad() 
	: LaunchStrength(1500),
	LaunchPitchAngle(35.f)
{
	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetBoxExtent(FVector(75, 75, 50));
	RootComponent = OverlapComp;
	//bind to event
	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::OverlapLaunchPad);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
}

void AFPSLaunchPad::OverlapLaunchPad(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// make rotator with our specified pitch and convert to a direction vector * intensity
	FRotator LaunchDirection = GetActorRotation();
	LaunchDirection.Pitch += LaunchPitchAngle;
	FVector LaunchVelocity = LaunchDirection.Vector() * LaunchStrength;

	if (ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor))
	{
		// launch player. both booleans give consistent launch velocity by ignoring the current player velocity
		OtherCharacter->LaunchCharacter(LaunchVelocity, true, true);

		// spawn fx
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivateLaunchPadEffectComp, GetActorLocation());
	}
	else if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);

		// spawn fx
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivateLaunchPadEffectComp, GetActorLocation());
	}
}
