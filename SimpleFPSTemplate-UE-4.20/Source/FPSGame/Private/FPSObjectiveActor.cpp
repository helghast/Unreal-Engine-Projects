// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSObjectiveActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FPSCharacter.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor() : PickUpFXComp(nullptr)
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(MeshComp);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();

	PlayEffects();
}

void AFPSObjectiveActor::PlayEffects()
{
	if (PickUpFXComp == nullptr && PickupFX)
	{
		PickUpFXComp = UGameplayStatics::SpawnEmitterAtLocation(this, PickupFX, GetActorLocation());
	}
}

void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	PlayEffects();

	if (Role == ROLE_Authority)
	{
		if (AFPSCharacter* MyChar = Cast<AFPSCharacter>(OtherActor))
		{
			MyChar->bIsCarryingObjective = true;

			PickUpFXComp->DestroyComponent();
			Destroy();
		}
	}
}
