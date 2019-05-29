// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectPooler.h"
#include "MyStaticMeshActor.h"

// Sets default values
AObjectPooler::AObjectPooler() : bFinished(false)
{

}

// Called when the game starts or when spawned
void AObjectPooler::BeginPlay()
{
	Super::BeginPlay();
	
	for (TArray<FPool>::TConstIterator It(pools); It; ++It)
	{
		TQueue<IPooledObject*>* objectPool = new TQueue<IPooledObject*>();

		for (int32 i = 0; i < It->size; ++i)
		{
			struct FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			IPooledObject* obj = GetWorld()->SpawnActor<AMyStaticMeshActor>(It->prefab, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
			if (obj)
			{
				obj->OnSetBaseProperties(true, false, false);
				objectPool->Enqueue(obj);
			}
		}

		poolDictionary.Add(It->tag, objectPool);
	}

	bFinished = true;
}

IPooledObject* AObjectPooler::SpawnFromPool(const EObjectType tag, const FVector& position, const FQuat& rotation)
{
	if (!poolDictionary.Contains(tag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Pool with tag %d doesnt exist"), static_cast<int>(tag));
		return nullptr;
	}

	IPooledObject* objectToSpawn = nullptr;
	const bool dequed = poolDictionary[tag]->Dequeue(objectToSpawn);
	if (dequed && objectToSpawn)
	{
		objectToSpawn->OnSetBaseProperties(false, true, false);
		objectToSpawn->OnSetLocationAndRotation(position, rotation);
		objectToSpawn->OnObjectSpawn();

		poolDictionary[tag]->Enqueue(objectToSpawn);

		return objectToSpawn;
	}

	return nullptr;
}
