// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolUtils.h"
#include "Pool.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct OBJECTPOOLINGPATTERN_API FPool
{
	GENERATED_BODY()

public:
	FPool();

	UPROPERTY(EditDefaultsOnly, Category = Pool)
	EObjectType tag;

	UPROPERTY(EditDefaultsOnly, Category = Pool)
	TSubclassOf<class AMyStaticMeshActor> prefab;

	UPROPERTY(EditDefaultsOnly, Category = Pool)
	int32 size;
};
