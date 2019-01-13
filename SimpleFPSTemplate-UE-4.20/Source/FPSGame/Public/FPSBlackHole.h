// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSBlackHole.generated.h"

UCLASS()
class FPSGAME_API AFPSBlackHole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSBlackHole();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = Components)
	class UStaticMeshComponent* MeshComp;

	// Inner sphere destroys the overlapping components
	UPROPERTY(VisibleAnywhere, Category = Components)
	class USphereComponent* InnerSphereComp;

	// outher sphere pulls components (that are physically simulating) towards the centre of the actor
	UPROPERTY(VisibleAnywhere, Category = Components)
	class USphereComponent* OutherSphereComp;

	UFUNCTION()
	void OverlapInnerSphere(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFrowmSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
