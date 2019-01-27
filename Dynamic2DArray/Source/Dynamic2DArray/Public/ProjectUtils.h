// Fill out your copyright notice in the Description page of Project Settings.
/** https://answers.unrealengine.com/questions/551199/dynamic-2d-array-using-tarray.html */

#pragma once
#include "CoreMinimal.h"
#include "ProjectUtils.generated.h"

/** most basic struct for the example */
USTRUCT(BlueprintType)
struct FItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
	bool isEquipped = false;
};

/** use a 1D-array of struct FPockets, each of them will store an 1D-array of FItems, to simulate a 2D-array. */
USTRUCT(BlueprintType)
struct FPocket
{
	GENERATED_BODY()

public:
	/** 1D-array of structs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ArryItems)
	TArray<FItem> Items;

	/** Array bracket operator. Returns reference to element at give index. */
	FItem& operator[](const int32 Index)
	{
		return Items[Index];
	}

	/** Array bracket operator. Returns reference to element at give index. Const version of the above. */
	const FItem& operator[](const int32 Index) const
	{
		return Items[Index];
	}

	/** add a copy of the item reference to the items array */
	void Add(const FItem& Value)
	{
		Items.Add(Value);
	}
};

/**
 * 
 */
class DYNAMIC2DARRAY_API ProjectUtils
{
public:
	ProjectUtils();
	~ProjectUtils();
};
