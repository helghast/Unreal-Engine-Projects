// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSExtractionZone.generated.h"

UCLASS()
class FPSGAME_API AFPSExtractionZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSExtractionZone();

protected:
	UPROPERTY(VisibleAnywhere, Category = Components)
	class UBoxComponent* OverlapComp;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UDecalComponent* DecalComp;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	class USoundBase* ObjectiveMissingSound;

	UFUNCTION()
	void HandleOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
