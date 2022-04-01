// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectRogue : ModuleRules
{
	public ProjectRogue(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore", "HeadMountedDisplay", "UMG" });	}
}
