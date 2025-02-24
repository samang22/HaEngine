#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IRendererModule;

/** 엔진 모듈을 구현합니다. */
class FEngineModule : public FDefaultModuleImpl
{
public:
    // IModuleInterface
    virtual void StartupModule();
    virtual void ShutdownModule();
};