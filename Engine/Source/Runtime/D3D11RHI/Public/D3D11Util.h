#pragma once
#include "CoreMinimal.h"

class ID3D11Device;

/*=============================================================================
	D3D11Util.h: D3D RHI utility definitions.
=============================================================================*/

//#define D3D11RHI_IMMEDIATE_CONTEXT	(GD3D11RHI->GetDeviceContext())
//#define D3D11RHI_DEVICE				(GD3D11RHI->GetDevice())

/**
 * 결과가 실패가 아닌지 확인합니다. 실패한 경우 애플리케이션은 종료되지 않고 적절한 오류 메시지만 로그합니다.
 * @param   Result - 확인할 결과 코드입니다.
 * @param   Code - 결과를 생성한 코드입니다.
 * @param   Filename - Code를 포함하는 소스 파일의 파일 이름입니다.
 * @param   Line - Filename 내에서 Code의 줄 번호입니다.
 */
extern D3D11RHI_API void VerifyD3D11ResultNoExit(HRESULT Result, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line, ID3D11Device* Device);

/**
 * 결과가 실패가 아닌지 확인합니다. 실패한 경우 애플리케이션은 적절한 오류 메시지와 함께 종료됩니다.
 * @param   Result - 확인할 결과 코드입니다.
 * @param   Code - 결과를 생성한 코드입니다.
 * @param   Filename - Code를 포함하는 소스 파일의 파일 이름입니다.
 * @param   Line - Filename 내에서 Code의 줄 번호입니다.
 */
extern D3D11RHI_API void VerifyD3D11Result(HRESULT Result, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line, ID3D11Device* Device);


/**
 * A macro for using VERIFYD3D11RESULT that automatically passes in the code and filename/line.
 */
#define VERIFYD3D11RESULT_EX(x, Device)	{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11Result(hr,#x,__FILE__,__LINE__, Device); }}
#define VERIFYD3D11RESULT(x)			{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11Result(hr,#x,__FILE__,__LINE__, 0); }}
#define VERIFYD3D11RESULT_NOEXIT(x)		{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11ResultNoExit(hr,#x,__FILE__,__LINE__, 0); }}
//#define VERIFYD3D11SHADERRESULT(Result, Shader, Device) {HRESULT hr = (Result); if (FAILED(hr)) { VerifyD3D11ShaderResult(Shader, hr, #Result,__FILE__,__LINE__, Device); }}
#define VERIFYD3D11RESULT_NOEXIT(x)		{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11ResultNoExit(hr,#x,__FILE__,__LINE__, 0); }}
//#define VERIFYD3D11CREATETEXTURERESULT(x,UEFormat,SizeX,SizeY,SizeZ,Format,NumMips,Flags,Usage,CPUAccessFlags,MiscFlags,SampleCount,SampleQuality,SubResPtr,SubResPitch,SubResSlicePitch,Device,DebugName) {HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11CreateTextureResult(hr, UEFormat,#x,__FILE__,__LINE__,SizeX,SizeY,SizeZ,Format,NumMips,Flags,Usage,CPUAccessFlags,MiscFlags,SampleCount,SampleQuality,SubResPtr,SubResPitch,SubResSlicePitch,Device,DebugName); }}
//#define VERIFYD3D11RESIZEVIEWPORTRESULT(x, OldState, NewState, Device) { HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11ResizeViewportResult(hr, #x, __FILE__, __LINE__, OldState, NewState, Device); }}
//#define VERIFYD3D11CREATEVIEWRESULT(x, Device, Resource, Desc) {HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11CreateViewResult(hr, #x, __FILE__, __LINE__, Device, Resource, Desc); }}
