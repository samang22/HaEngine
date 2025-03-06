cbuffer FObjectUniformBuffer : register(b0)
{
    matrix WorldMatrix;
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
};

FVSOutput VS(float3 Position : ATTRIBUTE0)
{
    float4 WorldPosition = mul(float4(Position, 1.f), WorldMatrix);
    //float4 ViewPosition = mul(WorldPosition, ViewMatrix);
    float4 FinalPosition = mul(WorldPosition, ViewProjectionMatrix);

    FVSOutput Output;
    Output.SVPosition = FinalPosition;
    Output.WorldPosition = WorldPosition;
    
    return Output;
}

float4 PS(float4 SVPosition : SV_Position, float4 WorldPosition : POSITIONW) : SV_Target0
{
    return float4(WorldPosition.xyz, 1.f);
}