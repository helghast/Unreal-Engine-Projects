// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileWeapon.h"

void ASProjectileWeapon::Fire()
{
	if (AActor* MyOwner = GetOwner())
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		struct FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);

		PlayCameraShake();
	}
}

