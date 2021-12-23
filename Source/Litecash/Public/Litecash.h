// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#if UE_BUILD_SHIPPING
#ifndef _WIN32
#include "GenericPlatform/GenericPlatformProcess.h"
#endif
#endif
#include "Modules/ModuleManager.h"

class FLitecashModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
