#include "PBRCommon.fxh"

struct FVSOutput
{
    float4 SVPosition : SV_Position;
    float4 WorldPosition : POSITIONW;
    float3 NormalW : NORMALW;
};

FVSOutput VS(float3 Position : ATTRIBUTE0, float3 Normal : ATTRIBUTE1)
{
    float4 WorldPosition = mul(float4(Position, 1.f), WorldMatrix);
    //float4 ViewPosition = mul(WorldPosition, ViewMatrix);
    float4 FinalPosition = mul(WorldPosition, ViewProjectionMatrix);

    FVSOutput Output;
    Output.SVPosition = FinalPosition;
    Output.WorldPosition = WorldPosition;
    Output.NormalW = normalize(mul(Normal, (float3x3) WorldInverseTransposeMatrix));
    
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
    //float3 N = normalize(Input.NormalW);
    //float3 L = LightDirection;
    // 물체에서 카메라로 향하는 방향 벡터를 계산한다
    const float3 V = normalize(EyePosition - Input.WorldPosition.xyz);
    
    //float Diffuse = max(dot(N, L), 0.1f);
    //return float4((LightColor * Diffuse).xyz, 1.f);
    
    // 물체의 Normal 벡터를 구한다
    const float3 N = normalize(Input.NormalW);
    const float AO = 1.f;
    
    // BaseColor
    float3 ConstantAlbedo = float3(1.f, 1.f, 1.f);
    float ConstantRoughness = 0.8f;
    float ConstantMetallic = 0.f;
    
    const float3 Color = LightSurface(V, N, LightColor.xyz, LightDirection, ConstantAlbedo, ConstantRoughness, ConstantMetallic, AO);

    return float4(Color, 1.f);
}