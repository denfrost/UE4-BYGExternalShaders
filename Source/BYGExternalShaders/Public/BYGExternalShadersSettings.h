// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ShaderCore.h"
#include "Misc/Paths.h"
#include "BYGExternalShadersSettings.generated.h"

#define LOCTEXT_NAMESPACE "FBYGExternalShadersModule"

USTRUCT()
struct FBYGExternalShaderEntry
{
	GENERATED_BODY()
public:
	// A virtual path. Must start with a /, must not end with a /, must not contain .
	UPROPERTY(EditAnywhere)
	FString VirtualPath = "/Example/Dir";

	// The physical directory that exists on your hard-drive.
	UPROPERTY(EditAnywhere, meta=(ContentDir))
	FDirectoryPath RealPath;

	FString GetRealPath() const
	{
		// Because RealPath is marked as ContentDir (for convenience for users)
		// the path we get back is something like "/Game/MyShaders/Blah"
		// To convert this to work with ProjectContentDir we need to cut off the /Game bit
		FString Path = RealPath.Path;
		if (ensure(RealPath.Path.StartsWith("/Game")))
			Path = RealPath.Path.RightChop(5);
		const FString OutPath = FPaths::RemoveDuplicateSlashes(FPaths::Combine(FPaths::ProjectContentDir(), Path));
		return OutPath;
	}

	bool IsValid() const
	{
		return GetErrors().Num() == 0;
	}

	TArray<FText> GetErrors() const
	{
		TArray<FText> Errors;
		// Copied from AddShaderSourceDirectoryMapping, we want to validate here so we don't get checkf as
		// the user is changing their directory stuff
		if (!VirtualPath.StartsWith(TEXT("/")))
			Errors.Add(LOCTEXT("RequiredSlashStart", "Virtual path must start with forward-slash /"));
		if (VirtualPath.EndsWith(TEXT("/")))
			Errors.Add(LOCTEXT("NoSlashEnd", "Virtual path must not end with a forward-slash /"));
		if (VirtualPath.Contains(FString(TEXT("."))))
			Errors.Add(LOCTEXT("NoPeriod", "Virtual path may not contain period ."));
		// No duplicates allowed
		if (AllShaderSourceDirectoryMappings().Contains(VirtualPath))
			Errors.Add(LOCTEXT("NoDuplicate", "An entry with this virtual path already exists"));
		if (!FPaths::DirectoryExists(GetRealPath()))
			Errors.Add(LOCTEXT("RequiredRealDirectory", "Real path must be a directory that exists."));

		return Errors;
	}
};

UCLASS(config = Game, defaultconfig)
class BYGEXTERNALSHADERS_API UBYGExternalShadersSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(config, EditAnywhere, Category = Settings, meta=(TitleProperty="VirtualPath", ConfigRestartRequired=true))
	TArray<FBYGExternalShaderEntry> Entries;
};

#undef LOCTEXT_NAMESPACE
