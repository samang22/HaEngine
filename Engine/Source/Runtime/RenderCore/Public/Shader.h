#pragma once
#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIDefinitions.h"
#include "RenderResource.h"


/**
 * 컴파일된 셰이더 및 그 파라미터 바인딩.
 */
class FShader
{
    friend class FShaderType;
    friend class FShaderCompilingManager;

private:
    TArray<uint8> Code;
};

/**
 * 특정 셰이더 클래스를 직렬화/역직렬화, 컴파일 및 캐시하는 데 사용되는 객체입니다.
 *
 * 셰이더 타입은 EShaderPlatform이나 퍼뮤테이션 ID와 같은 여러 차원에서 여러 FShader 인스턴스를 관리할 수 있습니다.
 * 셰이더 타입의 퍼뮤테이션 수는 단순히 GetPermutationCount()에 의해 제공됩니다.
 */
class FShaderType
{
    using FShaderConstructFunction = function<TObjectPtr<FShader>()>;
    friend class FShaderCompilingManager;
    friend class FD3D11DynamicRHI;

public:
    RENDERCORE_API FShaderType(const type_index ClassType, const TCHAR* InClassName, const FString InShaderFilePath,
        const char* InFunctionName, EShaderFrequency InShaderFrequency, FShaderConstructFunction InShaderConstructFunction);
    RENDERCORE_API virtual ~FShaderType();

private:
    const type_index TypeIndex;
    FString ClassName;
    FString ShaderFilePath;
    string FunctionName;
    EShaderFrequency Frequency;
    FShaderConstructFunction ShaderConstructFunction;
};

#define DECLARE_SHADER_TYPE(ShaderClass) \
public:\
    static shared_ptr<FShader> ConstructCompiledInstance() { return make_shared<ShaderClass>(); }

/** A macro to implement a shader type. */
#define IMPLEMENT_SHADER_TYPE(ShaderClass,ShaderFilePath,FunctionName,Frequency) \
    static FShaderType StaticType_##ShaderClass(typeid(ShaderClass), TEXT(#ShaderClass), ShaderFilePath, FunctionName, Frequency, ShaderClass::ConstructCompiledInstance);


class RENDERCORE_API FGlobalShaderMap
{
    friend class FShaderType;
    friend class FShaderCompilingManager;

private:
    static inline map<type_index, FShaderType*> ShaderTypes;
    static inline map<type_index, TObjectPtr<FShader>> Shaders; // 컴파일된 Shader
};