// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapons"), EConsoleVariableFlags::ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
	: MuzzleSocketName("MuzzleSocket"),
	TracerTargetName("BeamEnd"),
	MuzzleEffect(nullptr), 
	DefaultImpactEffect(nullptr),
	FleshImpactEffect(nullptr),
	TracerEffect(nullptr),
	BaseDamage(20.f), 
	LastFireTime(0.f),
	RateOfFire(600.f),
	TimeBetweenShots(0.f),
	BulletSpread(1.0f)
{
 	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f; // 100 by engine default
	MinNetUpdateFrequency = 33.0f; // 2 by engine default
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::Fire()
{
	if (Role < ROLE_Authority)
	{
		ServerFire();
		return;
	}

	// trace the world from pawn eyes to cosshair location
	if (AActor* MyOwner = GetOwner())
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		
		FVector ShotDirection = EyeRotation.Vector();

		// randomize shot directions or bullet spread
		float HalfRadians = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRadians, HalfRadians);

		const FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		// particle "target" parameter
		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		const UWorld* World = GetWorld();
		if (World->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			// blocking hit! process damage
			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			// apply effect and damage value depends of surface material type
			float ActualDamage = BaseDamage;

			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(World, EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);
		PlayCameraShake();

		//if (Role == ROLE_Authority)
		{
			HitScanTrace.SurfaceType = SurfaceType;
			HitScanTrace.TraceTo = TracerEndPoint;
		}

		LastFireTime = World->TimeSeconds;
	}
}

void ASWeapon::OnRep_HitScanTrace()
{
	// play cosmetic FX
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayCameraShake();
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::PlayFireEffects(const FVector& TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		if (UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation))
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}
}

void ASWeapon::PlayCameraShake()
{
	if (APawn* MyOwner = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(MyOwner->GetController()))
		{
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}

void ASWeapon::PlayImpactEffects(const EPhysicalSurface SurfaceType, const FVector& ImpactPoint)
{
	UParticleSystem* SelectedEffect = (SurfaceType == SURFACE_FLESHDEFAULT || SurfaceType == SURFACE_FLESHVULNERABLE) ? FleshImpactEffect : DefaultImpactEffect;

	if (SelectedEffect)
	{
		const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASWeapon::StartFire()
{
	const float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(ASWeapon, HitScanTrace);
}
