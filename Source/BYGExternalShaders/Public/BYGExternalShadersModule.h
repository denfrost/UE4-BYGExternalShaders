// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

BYGEXTERNALSHADERS_API DECLARE_LOG_CATEGORY_EXTERN(LogBYGExternalShaders, Log, All);

class FBYGExternalShadersModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
protected:
	bool HandleSettingsSaved();
	FText HandleStatus();
	void ValidateAndAddShaderDirectories();
};
