// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class Dynamic2DArrayEditorTarget : TargetRules
{
	public Dynamic2DArrayEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "Dynamic2DArray" } );
	}
}
