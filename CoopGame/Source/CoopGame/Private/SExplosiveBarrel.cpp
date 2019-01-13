// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplosiveBarrel.h"
#include "SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
	: bExploded(false),
	ExplosionImpulse(400.f),
	ExplosionEffect(nullptr),
	ExplosionMaterial(nullptr)
{
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	// set to physics body to let radial component affect us (eg. when a nearby barrel explodes)
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // prevent component from ticking, and only use fireimpulse() instead
	RadialForceComp->bIgnoreOwningActor = true; // ignore itself

	SetReplicates(true);
	SetReplicateMovement(true); // this can be expensive
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bExploded)
	{
		return; // nothing left to do, already exploded;
	}

	if (Health <= 0.0f)
	{
		// explode!
		bExploded = true; // replicated changes to client
		OnRep_Exploded(); // run on server too

		// START: if dont want to replicate Movement, SetReplicateMovement(false) and Cubes::Static Mesh Replicate Movement = false, move this code to OnRep_Exploded()
		// boost the barrel upwards
		const FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		// blast away nearby physics actors
		RadialForceComp->FireImpulse();
		// END

		// @todo: apply radial damage
	}
}

void ASExplosiveBarrel::OnRep_Exploded()
{
	// play FX and change self material to black
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	// override material on mesh with blackened version
	MeshComp->SetMaterial(0, ExplosionMaterial);
}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel, bExploded);
}
