// Fill out your copyright notice in the Description page of Project Settings.

#include "DummyActor.h"
#include "Kismet/KismetStringLibrary.h" // to convert bools to strings

// Sets default values
ADummyActor::ADummyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// add two pockets to the inventory
	Inventory.Add(FPocket());
	Inventory.Add(FPocket());

	// add one item to the pocket 1.
	Inventory[0].Add(FItem());

	// change isEquipped item property using [][] operators
	Inventory[0][0].isEquipped = true;
}

// Called when the game starts or when spawned
void ADummyActor::BeginPlay()
{
	Super::BeginPlay();

	// add in runtime new items
	Inventory[1].Add(FItem());
	Inventory[1].Add(FItem());
	Inventory[1].Add(FItem());
	Inventory[1].Add(FItem());

	// change properties
	Inventory[1][1].isEquipped = true;
	Inventory[1][2].isEquipped = true;

	// get a non-const reference to an item and update it
	FItem& Item = Inventory[1][3];
	Item.isEquipped = true;

	// iterate
	for (TArray<FPocket>::TConstIterator ItPocket(Inventory); ItPocket; ++ItPocket)
	{
		for (TArray<FItem>::TConstIterator ItItem(ItPocket->Items); ItItem; ++ItItem)
		{
			const FString Str = UKismetStringLibrary::Conv_BoolToString(ItItem->isEquipped);
			const FString Msg = FString::Printf(TEXT("[%d][%d].isEquipped = %s"), ItPocket.GetIndex(), ItItem.GetIndex(), *Str);

			// show message on the screen
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);

			// show message on the outputlog
			UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
		}
	}
}

// Called every frame
void ADummyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

