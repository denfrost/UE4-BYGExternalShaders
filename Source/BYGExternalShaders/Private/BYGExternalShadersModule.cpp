// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGExternalShadersModule.h"
#include "BYGExternalShadersSettings.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FBYGExternalShadersModule"

DEFINE_LOG_CATEGORY(LogBYGExternalShaders);

void FBYGExternalShadersModule::StartupModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings(
			"Project", "Plugins", "BYG External Shaders",
			LOCTEXT("RuntimeGeneralSettingsName", "BYG External Shaders"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Configure directories from which to load shader code."),
			GetMutableDefault<UBYGExternalShadersSettings>()
			);

		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FBYGExternalShadersModule::HandleSettingsSaved);
			SettingsSection->OnStatus().BindRaw(this, &FBYGExternalShadersModule::HandleStatus);
		}
	}
	ValidateAndAddShaderDirectories();
}

void FBYGExternalShadersModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "BYG External Shaders");
	}
}

bool FBYGExternalShadersModule::HandleSettingsSaved()
{
	UBYGExternalShadersSettings* Settings = GetMutableDefault<UBYGExternalShadersSettings>();
	bool ResaveSettings = false;

	// Pretty sure we can't just re-run adding the stuff when the settings are saved because
	// a) we'd get duplicates
	// b) if we wipe the mappings using the function below... it seems overkill
	// Ideally we'd just remove our own but that's for later.
	//ResetAllShaderSourceDirectoryMappings();
	//ValidateAndAddShaderDirectories();

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

	// TODO re-add any modified stuff?

	return true;
}

FText FBYGExternalShadersModule::HandleStatus()
{
	TArray<FString> StatusLines;
	const UBYGExternalShadersSettings& Settings = *GetDefault<UBYGExternalShadersSettings>();
	for (int32 i = 0; i < Settings.Entries.Num(); ++i)
	{
		const auto& Entry = Settings.Entries[i];
		if (!Entry.IsValid())
		{
			StatusLines.Add(FString::Printf(TEXT("%d: '%s' has invalid path '%s'"), i, *Entry.VirtualPath, *Entry.GetRealPath()));
		}
	}
	return FText::FromString(FString::Join(StatusLines, TEXT("\n")));
}


void FBYGExternalShadersModule::ValidateAndAddShaderDirectories()
{
	const UBYGExternalShadersSettings& Settings = *GetDefault<UBYGExternalShadersSettings>();
	for (const auto& Entry : Settings.Entries)
	{
		if (Entry.IsValid())
		{
			AddShaderSourceDirectoryMapping(Entry.VirtualPath, Entry.GetRealPath());
		}
		else
		{
			for (const FText Error : Entry.GetErrors())
			{
				UE_LOG(LogBYGExternalShaders, Error, TEXT("Invalid entry in BYG External Shader Settings: '%s' has error: '%s'"), *Entry.VirtualPath, *Error.ToString());
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBYGExternalShadersModule, BYGExternalShaders)
