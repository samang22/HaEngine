#pragma once
#include "CoreMinimal.h"
#include "Shader.h"
#include "Material.generated.h"

class FMaterialVS : public FShader
{
    DECLARE_SHADER_TYPE(FMaterialVS)
};
class FMaterialPS : public FShader
{
    DECLARE_SHADER_TYPE(FMaterialPS)
};

/**
 * 재질(Material)은 씬의 시각적 외형을 제어하기 위해 메쉬에 적용할 수 있는 에셋입니다.
 * 씬의 빛이 표면에 닿을 때, 재질의 셰이딩 모델을 사용하여 그 빛이 표면과 상호 작용하는 방식을 계산합니다.
 *
 * 경고: 새로운 재질을 직접 생성하면 셰이더 컴파일 시간이 증가합니다! 기존 재질을 사용하여 재질 인스턴스를 만드는 것을 고려하세요.
 */
UCLASS()
class UMaterial : public UObject
{
    GENERATED_BODY()

public:
    void SetVertexShader(TShaderMapRef<FMaterialVS> InShader);
    void SetPixelShader(TShaderMapRef<FMaterialPS> InShader);

    FRHIVertexShader* GetVertexShaderRHI() const;
    FRHIPixelShader* GetPixelShaderRHI() const;

private:
    TShaderMapRef<FMaterialVS> VertexShader;
    TShaderMapRef<FMaterialPS> PixelShader;
};