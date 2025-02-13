#include "D3D11RHIPrivate.h"

IRHICommandContext* FD3D11DynamicRHI::RHIGetDefaultContext()
{
	return this;
}

/**
 * 바운드 셰이더 상태를 설정합니다. 이 작업은 버텍스 선언/셰이더 및 픽셀 셰이더를 설정합니다.
 * @param BoundShaderState - 상태 리소스
 */
void FD3D11DynamicRHI::RHISetBoundShaderState(FRHIBoundShaderState* BoundShaderStateRHI)
{
    FD3D11BoundShaderState* BoundShaderState = ResourceCast(BoundShaderStateRHI);

    StateCache.SetStreamStrides(BoundShaderState->StreamStrides);
    StateCache.SetInputLayout(BoundShaderState->InputLayout);
    StateCache.SetVertexShader(BoundShaderState->VertexShader);
    StateCache.SetPixelShader(BoundShaderState->PixelShader);

    //StateCache.SetGeometryShader(BoundShaderState->GeometryShader);

    //// @TODO : really should only discard the constants if the shader state has actually changed.
    //bDiscardSharedConstants = true;

    //// Prevent transient bound shader states from being recreated for each use by keeping a history of the most recently used bound shader states.
    //// The history keeps them alive, and the bound shader state cache allows them to am be reused if needed.
    //BoundShaderStateHistory.Add(BoundShaderState);

    //// Shader changed so all resource tables are dirty
    //DirtyUniformBuffers[SF_Vertex] = 0xffff;
    //DirtyUniformBuffers[SF_Pixel] = 0xffff;
    //DirtyUniformBuffers[SF_Geometry] = 0xffff;

    //// Shader changed.  All UB's must be reset by high level code to match other platforms anway.
    //// Clear to catch those bugs, and bugs with stale UB's causing layout mismatches.
    //// Release references to bound uniform buffers.
    //for (int32 Frequency = 0; Frequency < SF_NumStandardFrequencies; ++Frequency)
    //{
    //    for (int32 BindIndex = 0; BindIndex < MAX_UNIFORM_BUFFERS_PER_SHADER_STAGE; ++BindIndex)
    //    {
    //        BoundUniformBuffers[Frequency][BindIndex] = nullptr;
    //    }
    //}

    //if (GUnbindResourcesBetweenDrawsInDX11 || GRHIGlobals.IsDebugLayerEnabled)
    //{
    //    ClearAllShaderResources();
    //}
}