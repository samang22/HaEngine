#include "D3D11RHIPrivate.h"

D3D11RHI_API map<type_index, TRefCountPtr<FRHIShader>> RHIShaders;

FVertexShaderRHIRef FD3D11DynamicRHI::RHICreateVertexShader(const TArray<uint8> Code, const type_index& Key)
{
    if (RHIShaders.contains(Key))
    {
        return (FVertexShaderRHIRef)RHIShaders[Key];
    }

    FD3D11VertexShader* Shader = new FD3D11VertexShader;
    VERIFYD3D11SHADERRESULT(Direct3DDevice->CreateVertexShader(Code.data(), Code.size(), nullptr, Shader->Resource.GetInitReference())
        , Shader, Direct3DDevice);
    Shader->Code = Code;

    FVertexShaderRHIRef ShaderRef = Shader;
    RHIShaders.emplace(Key, ShaderRef);

    return ShaderRef;
}

FPixelShaderRHIRef FD3D11DynamicRHI::RHICreatePixelShader(const TArray<uint8> Code, const type_index& Key)
{
    if (RHIShaders.contains(Key))
    {
        return (FPixelShaderRHIRef)RHIShaders[Key];
    }

    FD3D11PixelShader* Shader = new FD3D11PixelShader;
    VERIFYD3D11SHADERRESULT(Direct3DDevice->CreatePixelShader(Code.data(), Code.size(), nullptr, Shader->Resource.GetInitReference())
        , Shader, Direct3DDevice);

    FPixelShaderRHIRef ShaderRef = Shader;
    RHIShaders.emplace(Key, ShaderRef);

    return ShaderRef;
}

// UE의 FBoundShaderStateLookupKey 대체할 목적으로 만듬
// 우리는 이부분을 간소화 해서 처리 할 예정
struct FBoundShaderStateKey
{
	uint32 Hash;
	FRHIVertexDeclaration* VertexDeclaration;
	FRHIVertexShader* VertexShader;
	FRHIPixelShader* PixelShader;

	explicit FBoundShaderStateKey(FRHIVertexDeclaration* InVertexDeclaration, FRHIVertexShader* InVertexShader, FRHIPixelShader* InPixelShader)
		: VertexDeclaration(InVertexDeclaration), VertexShader(InVertexShader), PixelShader(InPixelShader)
	{
		FD3D11VertexDeclaration* D3D11VertexDeclaration = FD3D11DynamicRHI::ResourceCast(VertexDeclaration);
		Hash = D3D11VertexDeclaration->Hash;

		FD3D11VertexShader* D3D11VertexShader = FD3D11DynamicRHI::ResourceCast(VertexShader);
		FD3D11PixelShader* D3D11PixelShader = FD3D11DynamicRHI::ResourceCast(PixelShader);
		Hash += (uint32)(D3D11VertexShader->Resource.GetReference());
		Hash += (uint32)(D3D11PixelShader->Resource.GetReference());
	}

	bool operator==(const FBoundShaderStateKey& Other) const
	{
		return Hash == Other.Hash;
	}
};

namespace std {
	template<> struct hash<FBoundShaderStateKey> {
		std::size_t operator()(const FBoundShaderStateKey& s) const {
			return s.Hash;
		}
	};
}

typedef unordered_map<FBoundShaderStateKey, FBoundShaderStateRHIRef> FBoundShaderStateCache;
static FBoundShaderStateCache GBoundShaderStateCache;

FD3D11BoundShaderState::FD3D11BoundShaderState(FRHIVertexDeclaration* InVertexDeclarationRHI, FRHIVertexShader* InVertexShaderRHI, FRHIPixelShader* InPixelShaderRHI, ID3D11Device* Direct3DDevice)
{
	FD3D11VertexDeclaration* InVertexDeclaration = FD3D11DynamicRHI::ResourceCast(InVertexDeclarationRHI);
	FD3D11VertexShader* InVertexShader = FD3D11DynamicRHI::ResourceCast(InVertexShaderRHI);
	FD3D11PixelShader* InPixelShader = FD3D11DynamicRHI::ResourceCast(InPixelShaderRHI);
	//FD3D11GeometryShader* InGeometryShader = FD3D11DynamicRHI::ResourceCast(InGeometryShaderRHI);

	if (!InVertexDeclaration)
	{
		_ASSERT(false);
		return;
	}
	memcpy(StreamStrides, InVertexDeclaration->StreamStrides, sizeof(StreamStrides));
	VERIFYD3D11RESULT_EX(
		Direct3DDevice->CreateInputLayout(
			InVertexDeclaration->VertexElements.data(),
			InVertexDeclaration->VertexElements.size(),
			InVertexShader->Code.data(),
			InVertexShader->Code.size(),
			InputLayout.GetInitReference()
		),
		Direct3DDevice
	);

	VertexShader = InVertexShader->Resource;
	PixelShader = InPixelShader ? InPixelShader->Resource : nullptr;
	//GeometryShader = InGeometryShader ? InGeometryShader->Resource : nullptr;
}

FBoundShaderStateRHIRef FD3D11DynamicRHI::RHICreateBoundShaderState(
	FRHIVertexDeclaration* VertexDeclaration,
	FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader)
{
	FBoundShaderStateKey Key(VertexDeclaration, VertexShader, PixelShader);

	FBoundShaderStateRHIRef BoundShaderStateRefPtr = nullptr;
	auto It = GBoundShaderStateCache.find(Key);
	if (It == GBoundShaderStateCache.end())
	{
		auto Result = GBoundShaderStateCache.emplace(Key, new FD3D11BoundShaderState(VertexDeclaration, VertexShader, PixelShader, Direct3DDevice));

		BoundShaderStateRefPtr = Result.first->second;
	}
	else
	{
		BoundShaderStateRefPtr = It->second;
	}

	return BoundShaderStateRefPtr;
}