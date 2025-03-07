cbuffer FObjectUniformBuffer : register(b0)
{
    matrix WorldMatrix;
    matrix WorldInverseTransposeMatrix;
};

cbuffer FSceneUniformBuffer : register(b1)
{
    matrix ViewMatrix;
    matrix ProjectionMatrix;
    matrix ViewProjectionMatrix;
}

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

float4 PS(FVSOutput Input) : SV_Target0
{
    float3 N = normalize(Input.NormalW);
    float3 L = float3(-1.f, 0, 0);
    
    float Diffuse = max(dot(N, L), 0.f);
    return float4(Diffuse, Diffuse, Diffuse, 1.f);
}