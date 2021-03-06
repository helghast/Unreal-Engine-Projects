// Fill out your copyright notice in the Description page of Project Settings.

#include "MyStaticMeshActor.h"

AMyStaticMeshActor::AMyStaticMeshActor()
	: upForce(1.f),
	sideForce(0.1f), 
	LaunchAngle(45.f)
{

}

void AMyStaticMeshActor::OnObjectSpawn()
{
	if (GetStaticMeshComponent()->IsSimulatingPhysics())
	{
		const float xForce = FMath::RandRange(-sideForce, sideForce);
		const float yForce = FMath::RandRange(upForce * 0.5f, upForce);
		const float zForce = FMath::RandRange(-sideForce, sideForce);

		const FVector BoostIntesity(xForce, yForce, zForce);

		GetStaticMeshComponent()->AddImpulse(BoostIntesity, NAME_None, true);
	}
}

void AMyStaticMeshActor::OnSetBaseProperties(const bool Visible /*= false*/, const bool Collision /*= false*/, const bool Tick /*= false*/)
{
	SetActorHiddenInGame(Visible);
	SetActorEnableCollision(Collision);
	SetActorTickEnabled(Tick);
}

void AMyStaticMeshActor::OnSetLocationAndRotation(const FVector& position /*= FVector()*/, const FQuat& rotation /*= FQuat()*/)
{
	SetActorLocationAndRotation(position, rotation.GetNormalized());
}
