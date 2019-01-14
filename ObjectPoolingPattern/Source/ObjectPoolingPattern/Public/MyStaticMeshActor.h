// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PooledObject.h"
#include "MyStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class OBJECTPOOLINGPATTERN_API AMyStaticMeshActor : public AStaticMeshActor, public IPooledObject
{
	GENERATED_BODY()
	
public:
	AMyStaticMeshActor();

	UPROPERTY(EditDefaultsOnly, Category = Prefab)
	float upForce;

	UPROPERTY(EditDefaultsOnly, Category = Prefab)
	float sideForce;

	UPROPERTY(EditDefaultsOnly, Category = Prefab)
	float LaunchAngle;

	UFUNCTION()
	void OnObjectSpawn() override;
};
