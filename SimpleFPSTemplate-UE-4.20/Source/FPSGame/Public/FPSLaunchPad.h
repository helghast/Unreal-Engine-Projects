// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"

UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLaunchPad();

protected:
	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UBoxComponent* OverlapComp;

	// marked this function as ufunction to bind to overlap event
	UFUNCTION()
	void OverlapLaunchPad(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// total impulse added to the character on overlap. marked editinstanceonly to allow inlevel editing of this property per instance
	UPROPERTY(EditInstanceOnly, Category = LaunchPad)
	float LaunchStrength;

	// angle added on top of actor rotation to launch the character. marked editinstanceonly to allow inlevel editing of this property per instance
	UPROPERTY(EditInstanceOnly, Category = LaunchPad)
	float LaunchPitchAngle;

	// effect to apply when activating launch pad
	UPROPERTY(EditDefaultsOnly, Category = LaunchPad)
	class UParticleSystem* ActivateLaunchPadEffectComp;
};
