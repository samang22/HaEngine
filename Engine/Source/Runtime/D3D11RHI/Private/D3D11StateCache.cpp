
// DeviceContext 상태 캐싱 구현을 통해 불필요한 기기 컨텍스트 호출을 제거하여
// 렌더링 스레드 성능을 개선합니다.

//-----------------------------------------------------------------------------
//	Include Files
//-----------------------------------------------------------------------------
#include "D3D11RHIPrivate.h"
#include "D3D11StateCache.h"

void FD3D11StateCache::ClearState()
{
	if (Direct3DDeviceIMContext)
	{
		Direct3DDeviceIMContext->ClearState();
	}
}
