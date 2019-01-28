// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "TestStruct.generated.h"

USTRUCT()
struct FTestSubStruct
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SubInt;
	UPROPERTY()
	FString SubString;
};

USTRUCT()
struct FTestStruct
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value;
	UPROPERTY()
	FString Label;	
	UPROPERTY()
	TArray<FTestSubStruct> SubStruct;
};

USTRUCT()
struct FTestStructWithParameter
{
	GENERATED_BODY()

	UPROPERTY()
	FTestStruct TestStruct;	
};