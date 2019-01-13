// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSBlackHole.h"
#include "Components/SphereComponent.h"

// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	InnerSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComp"));
	InnerSphereComp->SetSphereRadius(100);
	InnerSphereComp->SetupAttachment(MeshComp);
	InnerSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSBlackHole::OverlapInnerSphere);

	OutherSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("OutherSphereComp"));
	OutherSphereComp->SetSphereRadius(3000);
	OutherSphereComp->SetupAttachment(MeshComp);
}

// Called when the game starts or when spawned
void AFPSBlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFPSBlackHole::OverlapInnerSphere(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFrowmSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		OtherActor->Destroy();
	}
}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// find all overlapping components that can collide and may be physically simulating
	TArray<UPrimitiveComponent*> OverlappingComps;
	OutherSphereComp->GetOverlappingComponents(OverlappingComps);

	for (TArray<UPrimitiveComponent*>::TConstIterator It(OverlappingComps); It; ++It)
	{
		UPrimitiveComponent* PrimComp = *It;
		if (PrimComp && PrimComp->IsSimulatingPhysics())
		{
			// the component we are looking for! it needs to be simulating in order to apply forces
			const float SphereRadius = OutherSphereComp->GetScaledSphereRadius();
			const float ForceStrength = -2000; // negative value to make it pull towards the origin instead of pushing away
			PrimComp->AddRadialForce(GetActorLocation(), SphereRadius, ForceStrength, ERadialImpulseFalloff::RIF_Constant, true);
		}
	}

}

