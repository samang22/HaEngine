#include "ShaderCompiler.h"
#include "Shader.h"

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
        ShaderType;
    }
}