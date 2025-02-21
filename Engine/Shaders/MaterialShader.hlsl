cbuffer FObjectUniformBuffer : register(b0)
{
    matrix WorldMatrix;
};

cbuffer FSceneUniformBuffer : register(b1)
{
    matrix ViewMatrix;
    matrix ProjectionMatrix;
}

float4 VS(float3 Position : ATTRIBUTE0) : SV_Position
{
    float4 WorldPosition = mul(float4(Position, 1.f), WorldMatrix);
    float4 ViewPosition = mul(WorldPosition, ViewMatrix);
    float4 FinalPosition = mul(ViewPosition, ProjectionMatrix);
    return FinalPosition;
}

float4 PS() : SV_Target
{
    return float4(1.f, 0.f, 0.f, 1.f);
}