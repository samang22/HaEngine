#include "Common.fxh"

Texture2D<float4> GBufferA : register(t0);
Texture2D<float4> GBufferB : register(t1);
Texture2D<float4> GBufferC : register(t2);
Texture2D<float4> SceneColor : register(t3);
SamplerState GBufferSampler : register(s0);

VSOutput_PositionUV VS(VSInput_PositionUV Input)
{
    VSOutput_PositionUV Output;
    Output.SVPosition = float4(Input.Position, 1.0f);
    Output.UV = Input.UV;
    return Output;
}

float4 PS(VSOutput_PositionUV Input) : SV_TARGET
{
    const float2 UV = Input.UV;
    float4 OutputColor = SceneColor.Sample(GBufferSampler, UV);

    static const float PreviewWidth = 0.18f;
    static const float PreviewHeight = 0.24f;
    static const float BorderWidth = 0.012f;

    if (UV.y < PreviewHeight && UV.x < PreviewWidth * 3.0f)
    {
        const uint PreviewIndex = min((uint)(UV.x / PreviewWidth), 2u);
        const float2 PreviewUV = float2(
            frac(UV.x / PreviewWidth),
            UV.y / PreviewHeight);

        if (PreviewUV.x < BorderWidth || PreviewUV.x > 1.0f - BorderWidth ||
            PreviewUV.y < BorderWidth || PreviewUV.y > 1.0f - BorderWidth)
        {
            return float4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (PreviewIndex == 0u)
        {
            const float3 NormalWS = GBufferA.Sample(GBufferSampler, PreviewUV).xyz;
            OutputColor = float4(NormalWS * 0.5f + 0.5f, 1.0f);
        }
        else if (PreviewIndex == 1u)
        {
            OutputColor = float4(GBufferB.Sample(GBufferSampler, PreviewUV).rgb, 1.0f);
        }
        else
        {
            OutputColor = float4(GBufferC.Sample(GBufferSampler, PreviewUV).rgb, 1.0f);
        }

        return OutputColor;
    }

    return OutputColor;
}
