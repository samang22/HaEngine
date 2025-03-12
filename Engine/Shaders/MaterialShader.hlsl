#include "PBRCommon.fxh"


sampler TextureSampler : register(s0);
Texture2D<float4> BaseColorTexture : register(t0);
Texture2D<float3> NormalTexture : register(t1);
Texture2D<float4> MetallicTexture : register(t2);
Texture2D<float4> RoughnessTexture : register(t3);
Texture2D<float4> AmbientOcclusionTexture : register(t4);

struct FVSInput
{
    float3 Position : ATTRIBUTE0;
    float3 Normal : ATTRIBUTE1;
    float2 UV : ATTRIBUTE2;
};


struct FVSOutput
{
    float4 SVPosition : SV_Position;
    float4 PositionWS : POSITIONW;
    float3 NormalWS : NORMALW;
    float2 UV : TEXCOORD0;
};

FVSOutput VS(FVSInput Input)
{
    float4 WorldPosition = mul(float4(Input.Position, 1.f), WorldMatrix);
    //float4 ViewPosition = mul(WorldPosition, ViewMatrix);
    float4 FinalPosition = mul(WorldPosition, ViewProjectionMatrix);

    FVSOutput Output;
    Output.SVPosition = FinalPosition;
    Output.PositionWS = WorldPosition;
    Output.NormalWS = normalize(mul(Input.Normal, (float3x3) WorldInverseTransposeMatrix));
    Output.UV = Input.UV;
    
    return Output;
}

//float4 PS(FVSOutput Input) : SV_Target0
//{
//    float3 N = normalize(Input.NormalW);
//    float3 L = LightDirection;
    
//    float Diffuse = max(dot(N, L), 0.f);
//    return float4((LightColor * Diffuse).xyz, 1.f);
//}

float4 PS(FVSOutput Input) : SV_Target0
{
    // float3 N = normalize(Input.NormalWS);
    // float3 L = LightDirection;
    // 물체에서 카메라로 향하는 방향 벡터를 계산한다
    const float3 V = normalize(EyePosition - Input.PositionWS.xyz);
    
    //float Diffuse = max(dot(N, L), 0.1f);
    //return float4((LightColor * Diffuse).xyz, 1.f);
    
    // Texture에서 구한 Normal을 -1 ~ 1 범위로 변환
    float3 LocalNormal = BiasX2(NormalTexture.Sample(TextureSampler, Input.UV));
    // Texture에서 구한 Normal을 World Space로 변환한다
    float3 NormalWS = PeturbNormal(LocalNormal, Input.PositionWS.xyz, Input.NormalWS, Input.UV);
    
    // BaseColor
    const float3 BaseColor = BaseColorTexture.Sample(TextureSampler, Input.UV);
    const float Metallic = MetallicTexture.Sample(TextureSampler, Input.UV).r;
    const float Roughness = RoughnessTexture.Sample(TextureSampler, Input.UV).r;
    const float AO = AmbientOcclusionTexture.Sample(TextureSampler, Input.UV).r;
    
    const float3 Color = LightSurface(V, NormalWS, LightColor.xyz, LightDirection, BaseColor, Roughness, Metallic, AO);
    
    return float4(Color, 1.f);
}