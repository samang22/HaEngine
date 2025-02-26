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

float4 VS(float3 Position : ATTRIBUTE0) : SV_Position
{
    float4 WorldPosition = mul(float4(Position, 1.f), WorldMatrix);
    //float4 ViewPosition = mul(WorldPosition, ViewMatrix);
    float4 FinalPosition = mul(WorldPosition, ViewProjectionMatrix);
    return FinalPosition;
}

float4 PS() : SV_Target0
{
    return float4(1.f, 0.f, 0.f, 1.f);
}