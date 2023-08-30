// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TestProject_Fntastic : ModuleRules
{
	public TestProject_Fntastic(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "NavigationSystem", "Navmesh", "UMG", "Slate", "SlateCore", "AIModule"});
	}
}
