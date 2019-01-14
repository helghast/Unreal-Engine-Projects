// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "Pool.h"
#include "PoolUtils.h"
#include "ObjectPooler.generated.h"

UCLASS()
class OBJECTPOOLINGPATTERN_API AObjectPooler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectPooler();

	UPROPERTY(EditDefaultsOnly, Category = Pooler)
	TArray<FPool> pools;

	class AMyStaticMeshActor* SpawnFromPool(const EObjectType tag, const FVector& position, const FQuat& rotation);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TMap<EObjectType, TQueue<class AMyStaticMeshActor*>* > poolDictionary;
};
