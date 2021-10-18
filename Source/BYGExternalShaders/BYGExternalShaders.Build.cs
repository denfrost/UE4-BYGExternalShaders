// Copyright Brace Yourself Games. All Rights Reserved.

using UnrealBuildTool;

public class BYGExternalShaders : ModuleRules
{
	public BYGExternalShaders(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"RenderCore"
			}
			);
	}
}
