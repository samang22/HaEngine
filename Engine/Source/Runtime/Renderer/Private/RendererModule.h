#pragma once

#include "CoreMinimal.h"
#include "RendererInterface.h"
#include "Shader.h"

/** 렌더러 모듈 구현입니다. */
class FRendererModule final : public IRendererModule
{
public:
    FRendererModule();

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};