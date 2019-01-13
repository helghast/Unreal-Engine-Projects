// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
	: Health(0.f), 
	DefaultHealth(100.f),
	bIsDead(false),
	TeamNum(255)
{
	SetIsReplicated(true);
}

// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// only hook if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (AActor* MyOwner = GetOwner())
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}
	
	Health = DefaultHealth;
}

void USHealthComponent::OnRep_Health(const float OldHealth)
{
	const float Damage = Health - OldHealth;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::HandleTakeAnyDamage(class AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}

	// update health clamped
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Health Changed: %f"), Health);

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	// this only happens on the server. Server handles take damage
	if (bIsDead)
	{
		if (ASGameMode* GM = GetWorld()->GetAuthGameMode<ASGameMode>())
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);
}

void USHealthComponent::Heal(const float HealAmount)
{
	if (HealAmount <= 0.f || Health <= 0.f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.f, DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Health Changed: %f (+%f)"), Health, HealAmount);

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

bool USHealthComponent::IsFriendly(class AActor* A, class AActor* B)
{
	if (A == nullptr || B == nullptr)
	{
		// assume friendly
		return true;
	}

	USHealthComponent* HealthA = Cast<USHealthComponent>(A->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* HealthB = Cast<USHealthComponent>(B->GetComponentByClass(USHealthComponent::StaticClass()));

	if (HealthA == nullptr || HealthB == nullptr)
	{
		// assume friendly
		return true;
	}

	return HealthA->TeamNum == HealthB->TeamNum;
}


