# Serializate UStructs in Unreal Engine 4

https://api.unrealengine.com/INT/API/Runtime/Json/Serialization/index.html
https://api.unrealengine.com/INT/API/Runtime/Json/Serialization/FJsonSerializer/index.html
https://api.unrealengine.com/INT/API/Runtime/JsonUtilities/FJsonObjectConverter/index.html
https://forums.unrealengine.com/development-discussion/c-gameplay-programming/1561415-solved-json-serializing-and-deserializing-c

to use json and json utils, goto projectname.build.cs and add Json and JsonUtilities:

	PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Json", "JsonUtilities" });

# Automation Tests in Unreal Engine 4

since 4.17 automation test are moved to plugins. then goto editor, plugins and enable Tests
https://docs.unrealengine.com/en-us/Programming/Automation
https://docs.unrealengine.com/en-us/Programming/Automation/TechnicalGuide
https://docs.unrealengine.com/en-us/Programming/Automation/UserGuide
