#include "D3D11RHIPrivate.h"
#include "D3D11Util.h"

#define D3DERR(x) case x: ErrorCodeText = TEXT(#x); break;

static FString GetD3D11DeviceHungErrorString(HRESULT ErrorCode)
{
	FString ErrorCodeText;

	switch (ErrorCode)
	{
		D3DERR(DXGI_ERROR_DEVICE_HUNG)
			D3DERR(DXGI_ERROR_DEVICE_REMOVED)
			D3DERR(DXGI_ERROR_DEVICE_RESET)
			D3DERR(DXGI_ERROR_DRIVER_INTERNAL_ERROR)
			D3DERR(DXGI_ERROR_INVALID_CALL)
	default: ErrorCodeText = FString(TEXT("{:X}"), (int32)ErrorCode);
	}

	return ErrorCodeText;
}


FString GetD3D11ErrorString(HRESULT ErrorCode, ID3D11Device* Device)
{
	FString ErrorCodeText;

	switch (ErrorCode)
	{
		D3DERR(S_OK);
		D3DERR(D3D11_ERROR_FILE_NOT_FOUND)
			D3DERR(D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
			D3DERR(E_FAIL)
			D3DERR(E_INVALIDARG)
			D3DERR(E_OUTOFMEMORY)
			D3DERR(DXGI_ERROR_INVALID_CALL)
			D3DERR(DXGI_ERROR_WAS_STILL_DRAWING)
			D3DERR(E_NOINTERFACE)
			D3DERR(DXGI_ERROR_DEVICE_REMOVED)
			D3DERR(DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
	default: ErrorCodeText = FString(TEXT("{:X}"), (int32)ErrorCode);
	}

	if (ErrorCode == DXGI_ERROR_DEVICE_REMOVED && Device)
	{
		HRESULT hResDeviceRemoved = Device->GetDeviceRemovedReason();
		ErrorCodeText += FString(TEXT(" ")) + GetD3D11DeviceHungErrorString(hResDeviceRemoved);
	}

	return ErrorCodeText;
}

void VerifyD3D11ResultNoExit(HRESULT D3DResult, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line, ID3D11Device* Device)
{
	_ASSERT(FAILED(D3DResult));

	const FString& ErrorString = GetD3D11ErrorString(D3DResult, Device);

	E_LOG(Error, TEXT("{} failed with error {}\n at {}:{}\n Error Code List: https://docs.microsoft.com/en-us/windows/desktop/direct3ddxgi/dxgi-error"), ANSI_TO_TCHAR(Code), ErrorString, ANSI_TO_TCHAR(Filename), Line);
}

void VerifyD3D11Result(HRESULT D3DResult, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line, ID3D11Device* Device)
{
	_ASSERT(FAILED(D3DResult));

	const FString& ErrorString = GetD3D11ErrorString(D3DResult, Device);

	E_LOG(Error, TEXT("{} failed with error {}\n at {}:{}"), ANSI_TO_TCHAR(Code), ErrorString, ANSI_TO_TCHAR(Filename), Line);

	//TerminateOnDeviceRemoved(D3DResult, Device);
	//TerminateOnOutOfMemory(D3DResult, false);

	E_LOG(Fatal, TEXT("{} failed with error {}\n at {}:{}"), ANSI_TO_TCHAR(Code), ErrorString, ANSI_TO_TCHAR(Filename), Line);
}