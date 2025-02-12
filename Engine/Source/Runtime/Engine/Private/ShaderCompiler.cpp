#include "ShaderCompiler.h"
#include "Shader.h"
#include "RHI.h"

ENGINE_API FShaderCompilingManager* FShaderCompilingManager::Get(const bool bDestroy)
{
    static unique_ptr<FShaderCompilingManager> Instance = make_unique<FShaderCompilingManager>();
    if (bDestroy)
    {
        Instance.reset();
        return nullptr;
    }

    return Instance.get();
}

FShaderCompilingManager::FShaderCompilingManager()
{
    for (auto It : FGlobalShaderMap::ShaderTypes)
    {
        FShaderType* ShaderType = It.second;

        TArray<uint8> Result;
        if (GDynamicRHI->RHICompileShader(ShaderType, Result))
        {
            TObjectPtr<FShader> NewShader = ShaderType->ShaderConstructFunction();
            NewShader->Code = std::move(Result);
            NewShader->Frequency = ShaderType->Frequency;
            FGlobalShaderMap::Shaders.emplace(ShaderType->TypeIndex, NewShader);
        }
    }
}