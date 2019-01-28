// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/KismetStringLibrary.h"
#include "JsonUtilities.h"

template <typename Type>
struct FSerializableStruct
{
public:

	/** Convert all the struct fields/properties to a json string */
	template <typename T>
	static FString Serialize(const T& Value)
	{
		FString JSONString;
		FJsonObjectConverter::UStructToJsonObjectString(Type::StaticStruct(), &Value, JSONString, 0, 0, 0);

		return CleanJSONString(JSONString);
	}

	/** Convert all the struct fields/properties to a json strings with a ID field */
	template <typename T>
	static FString SerializeWithParameter(const T& Value, const FString& ParameterName)
	{
		FString JSONString = FSerializableStruct<T>::Serialize(Value);

		TArray<FString> Strings;
		Strings.Add("{\"");
		Strings.Add(ParameterName);
		Strings.Add("\": ");
		Strings.Add(JSONString);
		Strings.Add("}");

		JSONString = UKismetStringLibrary::JoinStringArray(Strings, TEXT(""));
		return JSONString;
	}

	/** Remove special characters (return, newline, tab) */
	static FString CleanJSONString(FString& JSONString)
	{
		JSONString = JSONString.Replace(TEXT("\r"), TEXT(""));
		JSONString = JSONString.Replace(TEXT("\n"), TEXT(""));
		JSONString = JSONString.Replace(TEXT("\t"), TEXT(""));
		return JSONString;
	}

	/** Convert a Json String containing JSON formatted data to a struct */
	template <typename T>
	static bool Deserialize(const FString& JSONString, T& Value)
	{
		return FJsonObjectConverter::JsonObjectStringToUStruct<Type>(JSONString, &Value, 0, 0);
	}
};


/**
 * 
 */
class SERIALIZESTRUCT_API SerializationUtils
{
public:
	SerializationUtils();
	~SerializationUtils();
};
