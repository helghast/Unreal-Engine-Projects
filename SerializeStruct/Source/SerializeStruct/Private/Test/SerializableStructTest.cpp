// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "AutomationTest.h"
#include "TestStruct.h"
#include "SerializationUtils.h"

class SerializableStructTest
{
public:
	FString JSONString;
	FTestStruct StructTest;

	SerializableStructTest();
	bool CheckString(const FString& FindString);
};

SerializableStructTest::SerializableStructTest()
{
	StructTest.Value = 55;
	StructTest.Label = TEXT("Test");

	FTestSubStruct SubStruct;
	SubStruct.SubInt = 853;
	SubStruct.SubString = TEXT("Sub1");
	StructTest.SubStruct.Add(SubStruct);

	SubStruct.SubInt = 127;
	SubStruct.SubString = TEXT("Sub2");
	StructTest.SubStruct.Add(SubStruct);
}

bool SerializableStructTest::CheckString(const FString& FindString)
{
	const int32 foundLabel = JSONString.Find(FindString, ESearchCase::CaseSensitive);
	return foundLabel > 0;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SerialiableStructStandarSerializationTest, "TestGroup.SerializableStruct.StandarSerialization", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool SerialiableStructStandarSerializationTest::RunTest(const FString& Parameters)
{	
	SerializableStructTest test;
	test.JSONString = FSerializableStruct<FTestStruct>::Serialize(test.StructTest);

	//Main NotFound
	TestFalse("Found 1 Fail", test.CheckString("Value"));
	TestFalse("Found 2 Fail", test.CheckString("Label"));

	//Main Found
	TestTrue("Found 3 Fail", test.CheckString("value"));
	TestTrue("Found 4 Fail", test.CheckString("label"));

	//SubStruct Not Found
	TestFalse("Found Sub 1 Fail", test.CheckString("SubInt"));
	TestFalse("Found Sub 2 Fail", test.CheckString("SubString"));

	//SubStruct Found
	TestTrue("Found Sub 3 Fail", test.CheckString("subInt"));
	TestTrue("Found Sub 4 Fail", test.CheckString("subString"));

	FTestStruct structResult;
	FSerializableStruct<FTestStruct>::Deserialize(test.JSONString, structResult);

	TestEqual("Value Fail", 55, structResult.Value);
	TestEqual("Label Fail", (FString)TEXT("Test"), structResult.Label);

	TestEqual("SubStruct Fail", 2, structResult.SubStruct.Num());
	if (structResult.SubStruct.Num() == 0)
	{
		return false;
	}

	TestEqual("SubStruct 1 SubValue Fail", 853, structResult.SubStruct[0].SubInt);
	TestEqual("SubStruct 1 SubLabel Fail", (FString)TEXT("Sub1"), structResult.SubStruct[0].SubString);

	TestEqual("SubStruct 2 SubValue Fail", 127, structResult.SubStruct[1].SubInt);
	TestEqual("SubStruct 2 SubLabel Fail", (FString)TEXT("Sub2"), structResult.SubStruct[1].SubString);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SerialiableStructWithParameterTest, "TestGroup.SerializableStruct.SerializeWithParameter", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool SerialiableStructWithParameterTest::RunTest(const FString& Parameters)
{
	FTestStruct structTest;
	structTest.Value = 55;
	structTest.Label = TEXT("Test");

	const FString JSONString = FSerializableStruct<FTestStruct>::SerializeWithParameter(structTest, TEXT("TestStruct"));

	FTestStructWithParameter structResult;
	FSerializableStruct<FTestStructWithParameter>::Deserialize(JSONString, structResult);

	TestEqual("Value Fail", 55, structResult.TestStruct.Value);
	TestEqual("Label Fail", (FString)TEXT("Test"), structResult.TestStruct.Label);

	return true;
}
