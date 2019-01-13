// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "NavigationSystem/Public/NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"
#include "SCharacter.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"

static int32 DebugTrackerBotDrawing = 0;
FAutoConsoleVariableRef CVARDebugTrackerBotDrawing(TEXT("COOP.DebugTrackerBot"), DebugTrackerBotDrawing, TEXT("Draw Debug Lines for TrackerBot"), EConsoleVariableFlags::ECVF_Cheat);

// Sets default values
ASTrackerBot::ASTrackerBot()
	: NextPathPoint(EForceInit::ForceInit),
	bUseVelocityChange(false),
	MovementForce(1000.f),
	RequiredDistanceToTarget(100.f),
	ExplosionEffect(nullptr),
	bExploded(false),
	ExplosionRadius(350),
	ExplosionDamage(60),
	bStartSelfDestruction(false),
	SelfDestructSound(nullptr),
	ExplodeSound(nullptr),
	SelfDamageInterval(0.5f)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority)
	{
		//find initial move-to
		NextPathPoint = GetNextPathPoint();

		// every second we update our powerlevel based on nearby bots. challenge code
		FTimerHandle TimerHandle_CheckPowerLevel;
		GetWorldTimerManager().SetTimer(TimerHandle_CheckPowerLevel, this, &ASTrackerBot::OnCheckNearbyBots, 1.0f, true);
	}
}

void ASTrackerBot::HandleTakeDamage(class USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser)
{
	// only server updates health
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	//UE_LOG(LogTemp, Log, TEXT("Health %f of %s"), Health, *GetName());

	if (Health <= 0.f)
	{
		SelfDestruct();
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	// hack to get player location
	// ACharacter* Char = UGameplayStatics::GetPlayerCharacter(this, 0);

	AActor* BestTarget = nullptr;
	float NearestTargetDistance = FLT_MAX;

	for (FConstPawnIterator It(GetWorld()->GetPawnIterator()); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || USHealthComponent::IsFriendly(TestPawn, this))
		{
			continue;
		}

		USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (TestPawnHealthComp && TestPawnHealthComp->GetHealth() > 0.0f)
		{
			const float DistanceBetweenActors = (TestPawn->GetActorLocation() - GetActorLocation()).Size();
			if (DistanceBetweenActors < NearestTargetDistance)
			{
				BestTarget = TestPawn;
				NearestTargetDistance = DistanceBetweenActors;
			}
		}
	}

	if (BestTarget)
	{
		// navigation system exists only on server
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), /*Char*/BestTarget);

		// avoid stuck forever
		GetWorldTimerManager().ClearTimer(TimerHandle_RefreshPath);
		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &ASTrackerBot::RefreshPath, 5.0f, false);
		
		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			// return next point in the path
			return NavPath->PathPoints[1];
		}
	}

	// failed to find path
	return GetActorLocation();
}

void ASTrackerBot::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	const UWorld* World = GetWorld();
	UGameplayStatics::SpawnEmitterAtLocation(World, ExplosionEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		// increase damage based on the power level. challenge code
		float ActualDamage = ExplosionDamage + (ExplosionDamage * PowerLevel);

		// apply damage
		UGameplayStatics::ApplyRadialDamage(this, ActualDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		if (DebugTrackerBotDrawing)
		{
			DrawDebugSphere(World, GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.f, 0, 1.f);
		}

		// delete actor inmediately
		SetLifeSpan(2.0f);
	}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartSelfDestruction && !bExploded)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn && !USHealthComponent::IsFriendly(OtherActor, this))
		{
			// we overlaped with a player

			if (Role == ROLE_Authority)
			{
				// start self destruction sequence
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
			}

			bStartSelfDestruction = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// only relevant to server
	if (Role == ROLE_Authority && !bExploded)
	{
		const float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
		const UWorld* World = GetWorld();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			RefreshPath();

			if (DebugTrackerBotDrawing)
			{
				DrawDebugString(World, GetActorLocation(), "Target Reached!");
			}
		}
		else
		{
			// keep moving towards next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

			if (DebugTrackerBotDrawing)
			{
				DrawDebugDirectionalArrow(World, GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.f, 0, 1.f);
			}
		}

		if (DebugTrackerBotDrawing)
		{
			DrawDebugSphere(World, NextPathPoint, 20, 12, FColor::Yellow, false, 4.f, 1.f);
		}
	}
}

// challenge code

void ASTrackerBot::OnCheckNearbyBots()
{
	// distance to check for nearby bots
	const float Radius = 600.f;

	// create temporary collision shape for overlaps
	FCollisionShape CollShape;
	CollShape.SetSphere(Radius);

	// only find pawns, players and ai bots
	FCollisionObjectQueryParams QueryParams;
	// our tracker bot mesh component is set to physics body in blueprint. default profile of physics simulated actors
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);

	const UWorld* World = GetWorld();

	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	if (DebugTrackerBotDrawing)
	{
		DrawDebugSphere(World, GetActorLocation(), Radius, 12, FColor::White, false, 1.0f);
	}

	int32 NrOfBots = 0;
	// loop over the result using a "range base for loop"
	for (FOverlapResult Result : Overlaps)
	{
		// check if we overlapped with another tracker bot, ignoring players and other bot types
		ASTrackerBot* Bot = Cast<ASTrackerBot>(Result.GetActor());
		if (Bot && Bot != this)
		{
			NrOfBots++;
		}
	}

	const int32 MaxPowerLevel = 4;

	// clamp between min=0 and max=4
	PowerLevel = FMath::Clamp(NrOfBots, 0, MaxPowerLevel);

	// update the material color
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		// convert to a float between 0 and 1 just like "alpha" value of a texture. now the material can be set up without having to know the max power level
		// which can be tweaked many times by gameplay decisions. would mean we need to keep 2 places up to date
		float Alpha = PowerLevel / static_cast<float>(MaxPowerLevel);
		// note: cast converts int32 to a float
		// otherwise the following happens when dealing when dividing integers: 1 / 4 = 0 (powerlevel int / maxpoerlevel int = 0 int
		// this is a common programming problem and can be fixed by casting the int maxpowerlevel to a float before dividing.

		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}

	if (DebugTrackerBotDrawing)
	{
		// draw on the bot location
		DrawDebugString(World, FVector::ZeroVector, FString::FromInt(PowerLevel), this, FColor::White, 1.0f, true);
	}
}

// end challenge code