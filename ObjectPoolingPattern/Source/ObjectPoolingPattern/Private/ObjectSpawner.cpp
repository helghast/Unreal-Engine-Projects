// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectSpawner.h"
#include "ObjectPooler.h"
#include "PoolUtils.h"

// Sets default values
AObjectSpawner::AObjectSpawner() 
	: pooler(nullptr),
	DefaultType(EObjectType::Cube)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AObjectSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (poolerClass->IsValidLowLevel())
	{
		struct FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		pooler = GetWorld()->SpawnActor<AObjectPooler>(poolerClass, GetActorLocation(), FRotator::ZeroRotator);
	}
}

// Called every frame
void AObjectSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (pooler)
	{
		pooler->SpawnFromPool(DefaultType, GetActorLocation(), FQuat::Identity);
	}
}

