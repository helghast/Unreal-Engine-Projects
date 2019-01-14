// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectPooler.h"
#include "MyStaticMeshActor.h"

// Sets default values
AObjectPooler::AObjectPooler()
{

}

// Called when the game starts or when spawned
void AObjectPooler::BeginPlay()
{
	Super::BeginPlay();
	
	for (const FPool& pool : pools)
	{
		TQueue<AMyStaticMeshActor*>* objectPool = new TQueue<AMyStaticMeshActor*>();

		for (int32 i = 0; i < pool.size; ++i)
		{
			struct FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AMyStaticMeshActor* obj = GetWorld()->SpawnActor<AMyStaticMeshActor>(pool.prefab, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
			if (obj)
			{
				obj->SetActorHiddenInGame(true);
				obj->SetActorEnableCollision(false);
				obj->SetActorTickEnabled(false);

				objectPool->Enqueue(obj);
			}	
		}

		poolDictionary.Add(pool.tag, objectPool);
	}
}

AMyStaticMeshActor* AObjectPooler::SpawnFromPool(const EObjectType tag, const FVector& position, const FQuat& rotation)
{
	if (!poolDictionary.Contains(tag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Pool with tag %d doesnt exist"), static_cast<int>(tag));
		return nullptr;
	}

	AMyStaticMeshActor* objectToSpawn = nullptr;
	const bool dequed = poolDictionary[tag]->Dequeue(objectToSpawn);
	if (dequed && objectToSpawn)
	{
		objectToSpawn->SetActorHiddenInGame(false);
		objectToSpawn->SetActorEnableCollision(true);
		objectToSpawn->SetActorLocationAndRotation(position, rotation);

		// try use interface implementation
		if (IPooledObject* objImplementation = Cast<IPooledObject>(objectToSpawn))
		{
			objImplementation->OnObjectSpawn();
		}

		poolDictionary[tag]->Enqueue(objectToSpawn);

		return objectToSpawn;
	}

	return nullptr;
}
