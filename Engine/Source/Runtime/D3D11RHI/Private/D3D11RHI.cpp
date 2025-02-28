#include "D3D11RHIPrivate.h"

void FD3D11DynamicRHI::ClearState()
{
    StateCache.ClearState();
}

void FD3D11DynamicRHI::TrackResourceBoundAsVB(FD3D11ViewableResource* Resource, int32 StreamIndex)
{
    _ASSERT(StreamIndex >= 0 && StreamIndex < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
    if (Resource)
    {
        // 새로운 버텍스 버퍼(VB)를 바인딩합니다.
        // 가장 높은 바인딩된 리소스 인덱스로 최대 리소스 인덱스를 업데이트합니다.
        MaxBoundVertexBufferIndex = FMath::Max(MaxBoundVertexBufferIndex, StreamIndex);
        CurrentResourcesBoundAsVBs[StreamIndex] = Resource;
    }
    else if (CurrentResourcesBoundAsVBs[StreamIndex] != nullptr)
    {
        // 해당 슬롯에서 리소스를 언바인딩합니다.
        CurrentResourcesBoundAsVBs[StreamIndex] = nullptr;

        // 만약 이것이 가장 높은 바인딩된 리소스였다면...
        if (MaxBoundVertexBufferIndex == StreamIndex)
        {
            // 다음 non-null 슬롯을 찾거나 슬롯이 없을 때까지
            // 최대 리소스 인덱스를 하향 조정합니다.
            do
            {
                MaxBoundVertexBufferIndex--;
            } while (MaxBoundVertexBufferIndex >= 0 && CurrentResourcesBoundAsVBs[MaxBoundVertexBufferIndex] == nullptr);
        }
    }
}


void FD3D11DynamicRHI::TrackResourceBoundAsIB(FD3D11ViewableResource* Resource)
{
    CurrentResourceBoundAsIB = Resource;
}
