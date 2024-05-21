// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Portfolio : ModuleRules
{
	public Portfolio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        bEnableUndefinedIdentifierWarnings = false;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "AIModule", "GameplayTasks", "NavigationSystem", "Json", "Niagara" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        PublicIncludePaths.AddRange(new string[] {
            "../ThirdParty/Protobuf/include",
            "../ThirdParty/ProtoObject/include"
        });

        PublicAdditionalLibraries.AddRange(new string[] {
            "C:/Users/user/Documents/Unreal Projects/Portfolio/ThirdParty/Protobuf/lib/libprotobufd.lib",
            "C:/Users/user/Documents/Unreal Projects/Portfolio/ThirdParty/ProtoObject/lib/ProtoObject.lib"
        });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
