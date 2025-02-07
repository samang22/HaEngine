#pragma once
#include "CoreTypes.h"
#include "Math/SimpleMath.h"
#include "Templates/RefCounting.h"
#include "RHIDefinitions.h"
#include "RHICommandList.h"
#include "RHIAccess.h"

class FResourceBulkDataInterface;

enum class EClearBinding
{
	ENoneBound, //no clear color associated with this target.  Target will not do hardware clears on most platforms
	EColorBound, //target has a clear color bound.  Clears will use the bound color, and do hardware clears.
	EDepthStencilBound, //target has a depthstencil value bound.  Clears will use the bound values and do hardware clears.
};

struct FClearValueBinding
{
	struct DSVAlue
	{
		float Depth;
		uint32 Stencil;
	};

	FClearValueBinding()
		: ColorBinding(EClearBinding::EColorBound)
	{
		Value.Color[0] = 0.0f;
		Value.Color[1] = 0.0f;
		Value.Color[2] = 0.0f;
		Value.Color[3] = 0.0f;
	}

	FClearValueBinding(EClearBinding NoBinding)
		: ColorBinding(NoBinding)
	{
		_ASSERT(ColorBinding == EClearBinding::ENoneBound);

		Value.Color[0] = 0.0f;
		Value.Color[1] = 0.0f;
		Value.Color[2] = 0.0f;
		Value.Color[3] = 0.0f;

		Value.DSValue.Depth = 0.0f;
		Value.DSValue.Stencil = 0;
	}

	explicit FClearValueBinding(const FLinearColor& InClearColor)
		: ColorBinding(EClearBinding::EColorBound)
	{
		Value.Color[0] = InClearColor.R();
		Value.Color[1] = InClearColor.G();
		Value.Color[2] = InClearColor.B();
		Value.Color[3] = InClearColor.A();
	}

	explicit FClearValueBinding(float DepthClearValue, uint32 StencilClearValue = 0)
		: ColorBinding(EClearBinding::EDepthStencilBound)
	{
		Value.DSValue.Depth = DepthClearValue;
		Value.DSValue.Stencil = StencilClearValue;
	}

	FLinearColor GetClearColor() const
	{
		_ASSERT(ColorBinding == EClearBinding::EColorBound);
		return FLinearColor(Value.Color[0], Value.Color[1], Value.Color[2], Value.Color[3]);
	}

	void GetDepthStencil(float& OutDepth, uint32& OutStencil) const
	{
		_ASSERT(ColorBinding == EClearBinding::EDepthStencilBound);
		OutDepth = Value.DSValue.Depth;
		OutStencil = Value.DSValue.Stencil;
	}

	bool operator==(const FClearValueBinding& Other) const
	{
		if (ColorBinding == Other.ColorBinding)
		{
			if (ColorBinding == EClearBinding::EColorBound)
			{
				return
					Value.Color[0] == Other.Value.Color[0] &&
					Value.Color[1] == Other.Value.Color[1] &&
					Value.Color[2] == Other.Value.Color[2] &&
					Value.Color[3] == Other.Value.Color[3];

			}
			if (ColorBinding == EClearBinding::EDepthStencilBound)
			{
				return
					Value.DSValue.Depth == Other.Value.DSValue.Depth &&
					Value.DSValue.Stencil == Other.Value.DSValue.Stencil;
			}
			return true;
		}
		return false;
	}

	/*friend inline uint32 GetTypeHash(FClearValueBinding const& Binding)
	{
		uint32 Hash = GetTypeHash(Binding.ColorBinding);

		if (Binding.ColorBinding == EClearBinding::EColorBound)
		{
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[0]));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[1]));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[2]));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[3]));
		}
		else if (Binding.ColorBinding == EClearBinding::EDepthStencilBound)
		{
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.DSValue.Depth));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.DSValue.Stencil));
		}

		return Hash;
	}*/

	EClearBinding ColorBinding;

	union ClearValueType
	{
		float Color[4];
		DSVAlue DSValue;
	} Value;

	// common clear values
	static RHI_API const FClearValueBinding None;
	static RHI_API const FClearValueBinding Black;
	static RHI_API const FClearValueBinding BlackMaxAlpha;
	static RHI_API const FClearValueBinding White;
	static RHI_API const FClearValueBinding Transparent;
	static RHI_API const FClearValueBinding DepthOne;
	static RHI_API const FClearValueBinding DepthZero;
	static RHI_API const FClearValueBinding DepthNear;
	static RHI_API const FClearValueBinding DepthFar;
	static RHI_API const FClearValueBinding Green;
	static RHI_API const FClearValueBinding DefaultNormal8Bit;
};


/** 텍스처 리소스를 생성하는 데 사용되는 디스크립터 */
struct FRHITextureDesc
{
	FRHITextureDesc() = default;

	FRHITextureDesc(const FRHITextureDesc& Other)
	{
		*this = Other;
	}

	FRHITextureDesc(ETextureDimension InDimension)
		: Dimension(InDimension)
	{
	}

	FRHITextureDesc(
		ETextureDimension   InDimension
		, ETextureCreateFlags InFlags
		, EPixelFormat        InFormat
		, FClearValueBinding  InClearValue
		, FVector2D           InExtent
		, uint16              InDepth
		, uint16              InArraySize
		, uint8               InNumMips
		, uint8               InNumSamples
		, uint32              InExtData
	)
		: Flags(InFlags)
		, ClearValue(InClearValue)
		, ExtData(InExtData)
		, Extent(InExtent)
		, Depth(InDepth)
		, ArraySize(InArraySize)
		, NumMips(InNumMips)
		, NumSamples(InNumSamples)
		, Dimension(InDimension)
		, Format(InFormat)
	{
	}

	/*friend uint32 GetTypeHash(const FRHITextureDesc& Desc)
	{
		uint32 Hash = GetTypeHash(Desc.Dimension);
		Hash = HashCombine(Hash, GetTypeHash(Desc.Flags));
		Hash = HashCombine(Hash, GetTypeHash(Desc.Format));
		Hash = HashCombine(Hash, GetTypeHash(Desc.UAVFormat));
		Hash = HashCombine(Hash, GetTypeHash(Desc.Extent));
		Hash = HashCombine(Hash, GetTypeHash(Desc.Depth));
		Hash = HashCombine(Hash, GetTypeHash(Desc.ArraySize));
		Hash = HashCombine(Hash, GetTypeHash(Desc.NumMips));
		Hash = HashCombine(Hash, GetTypeHash(Desc.NumSamples));
		Hash = HashCombine(Hash, GetTypeHash(Desc.FastVRAMPercentage));
		Hash = HashCombine(Hash, GetTypeHash(Desc.ClearValue));
		Hash = HashCombine(Hash, GetTypeHash(Desc.ExtData));
		Hash = HashCombine(Hash, GetTypeHash(Desc.GPUMask.GetNative()));
		return Hash;
	}*/

	bool operator == (const FRHITextureDesc& Other) const
	{
		return Dimension == Other.Dimension
			&& Flags == Other.Flags
			&& Format == Other.Format
			&& UAVFormat == Other.UAVFormat
			&& Extent == Other.Extent
			&& Depth == Other.Depth
			&& ArraySize == Other.ArraySize
			&& NumMips == Other.NumMips
			&& NumSamples == Other.NumSamples
			&& FastVRAMPercentage == Other.FastVRAMPercentage
			&& ClearValue == Other.ClearValue
			&& ExtData == Other.ExtData;
		//&& GPUMask == Other.GPUMask;
	}

	bool operator != (const FRHITextureDesc& Other) const
	{
		return !(*this == Other);
	}

	FRHITextureDesc& operator=(const FRHITextureDesc& Other)
	{
		Dimension = Other.Dimension;
		Flags = Other.Flags;
		Format = Other.Format;
		UAVFormat = Other.UAVFormat;
		Extent = Other.Extent;
		Depth = Other.Depth;
		ArraySize = Other.ArraySize;
		NumMips = Other.NumMips;
		NumSamples = Other.NumSamples;
		ClearValue = Other.ClearValue;
		ExtData = Other.ExtData;
		FastVRAMPercentage = Other.FastVRAMPercentage;
		//GPUMask = Other.GPUMask;

		return *this;
	}

	bool IsTexture2D() const
	{
		return Dimension == ETextureDimension::Texture2D || Dimension == ETextureDimension::Texture2DArray;
	}

	bool IsTexture3D() const
	{
		return Dimension == ETextureDimension::Texture3D;
	}

	bool IsTextureCube() const
	{
		return Dimension == ETextureDimension::TextureCube || Dimension == ETextureDimension::TextureCubeArray;
	}

	bool IsTextureArray() const
	{
		return Dimension == ETextureDimension::Texture2DArray || Dimension == ETextureDimension::TextureCubeArray;
	}

	bool IsMipChain() const
	{
		return NumMips > 1;
	}

	bool IsMultisample() const
	{
		return NumSamples > 1;
	}

	FVector3D GetSize() const
	{
		return FVector3D(Extent.x, Extent.y, Depth);
	}

	void Reset()
	{
		// Usually we don't want to propagate MSAA samples.
		NumSamples = 1;

		// Remove UAV flag for textures that don't need it (some formats are incompatible).
		Flags |= TexCreate_RenderTargetable;
		Flags &= ~(TexCreate_UAV | TexCreate_ResolveTargetable | TexCreate_DepthStencilResolveTarget | TexCreate_Memoryless);
	}

	/** Returns whether this descriptor conforms to requirements. */
	//bool IsValid() const
	//{
	//	return FRHITextureDesc::Validate(*this, /* Name = */ TEXT(""), /* bFatal = */ false);
	//}

	/** RHI 텍스처에 전달되는 텍스처 플래그 */
	ETextureCreateFlags Flags = TexCreate_None;

	/** 텍스처를 빠르게 클리어할 때 사용할 클리어 값 */
	FClearValueBinding ClearValue;

	/* 다중 GPU 시스템에서 리소스를 생성할 GPU를 나타내는 마스크 */
	//FRHIGPUMask GPUMask = FRHIGPUMask::All();

	/** 플랫폼별 추가 데이터. 일부 플랫폼에서는 오프라인 처리된 텍스처에 사용됩니다. */
	uint32 ExtData = 0;

	// 정수로 사용하십시오
	/** 텍스처의 x 및 y 차원 */
	FVector3D Extent = FVector3D(1.f, 1.f, 0.f);

	/** 차원이 3D인 경우 텍스처의 깊이 */
	uint16 Depth = 1;

	/** 텍스처의 배열 요소 수. (차원이 3D인 경우 1로 유지) */
	uint16 ArraySize = 1;

	/** 텍스처 미프맵 체인의 미프 수 */
	uint8 NumMips = 1;

	/** 텍스처의 샘플 수. MSAA의 경우 >1 */
	uint8 NumSamples = 1;

	/** RHI 텍스처를 생성할 때 사용할 텍스처 차원 */
	ETextureDimension Dimension = ETextureDimension::Texture2D;

	/** RHI 텍스처를 생성할 때 사용할 픽셀 포맷 */
	EPixelFormat Format = PF_Unknown;

	/** UAV를 생성할 때 사용할 텍스처 포맷. PF_Unknown은 기본 포맷을 사용함을 의미(Format과 동일) */
	EPixelFormat UAVFormat = PF_Unknown;

	/** 리소스 메모리의 몇 퍼센트를 빠른 VRAM에 할당할지(hint-only). (8비트 인코딩, 0..255 -> 0%..100%) */
	uint8 FastVRAMPercentage = 0xFF;

	/** 유효성을 확인합니다. */
	//static bool CheckValidity(const FRHITextureDesc& Desc, const TCHAR* Name)
	//{
	//	return FRHITextureDesc::Validate(Desc, Name, /* bFatal = */ true);
	//}

	///**
	// * 설명된 텍스처가 GPU 메모리에서 차지할 예상 총 메모리 크기를 반환합니다.
	// * 이 값은 텍스처의 크기/포맷 등을 고려한 추정치입니다.
	// *
	// * 실행 중인 플랫폼 RHI의 텍스처 리소스 크기를 정확히 측정하려면 RHICalcTexturePlatformSize()를 사용하십시오.
	// *
	// * @param FirstMipIndex - 메모리 크기 계산에 고려할 가장 상세한 mip의 인덱스. NumMips보다 작고 LastMipIndex보다 작거나 같아야 합니다.
	// * @param LastMipIndex  - 메모리 크기 계산에 고려할 가장 간단한 mip의 인덱스. NumMips보다 작고 FirstMipIndex보다 크거나 같아야 합니다.
	// */
	//RHI_API uint64 CalcMemorySizeEstimate(uint32 FirstMipIndex, uint32 LastMipIndex) const;


	//uint64 CalcMemorySizeEstimate(uint32 FirstMipIndex = 0) const
	//{
	//	return CalcMemorySizeEstimate(FirstMipIndex, NumMips - 1);
	//}

//private:
	//RHI_API static bool Validate(const FRHITextureDesc& Desc, const TCHAR* Name, bool bFatal);
};


struct FRHITextureCreateDesc : public FRHITextureDesc
{
	static FRHITextureCreateDesc Create(const TCHAR* InDebugName, ETextureDimension InDimension)
	{
		return FRHITextureCreateDesc(InDebugName, InDimension);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture2D);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture2DArray);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture3D);
	}

	static FRHITextureCreateDesc CreateCube(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::TextureCube);
	}

	static FRHITextureCreateDesc CreateCubeArray(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::TextureCubeArray);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* DebugName, FVector3D Size, EPixelFormat Format)
	{
		return Create2D(DebugName)
			.SetExtent(Size)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* DebugName, int32 SizeX, int32 SizeY, EPixelFormat Format)
	{
		return Create2D(DebugName)
			.SetExtent(SizeX, SizeY)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* DebugName, FVector3D Size, uint16 ArraySize, EPixelFormat Format)
	{
		return Create2DArray(DebugName)
			.SetExtent(Size)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* DebugName, int32 SizeX, int32 SizeY, int32 ArraySize, EPixelFormat Format)
	{
		return Create2DArray(DebugName)
			.SetExtent(SizeX, SizeY)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* DebugName, FVector3D Size, EPixelFormat Format)
	{
		return Create3D(DebugName)
			.SetExtent(Size.x, Size.y)
			.SetDepth((uint16)Size.z)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* DebugName, int32 SizeX, int32 SizeY, int32 SizeZ, EPixelFormat Format)
	{
		return Create3D(DebugName)
			.SetExtent(SizeX, SizeY)
			.SetDepth((uint16)SizeZ)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc CreateCube(const TCHAR* DebugName, uint32 Size, EPixelFormat Format)
	{
		return CreateCube(DebugName)
			.SetExtent(Size)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc CreateCubeArray(const TCHAR* DebugName, uint32 Size, uint16 ArraySize, EPixelFormat Format)
	{
		return CreateCubeArray(DebugName)
			.SetExtent(Size)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	FRHITextureCreateDesc() = default;

	// Constructor with minimal argument set. Name and dimension are always required.
	FRHITextureCreateDesc(const TCHAR* InDebugName, ETextureDimension InDimension)
		: FRHITextureDesc(InDimension)
		, DebugName(InDebugName)
	{
	}

	// Constructor for when you already have an FRHITextureDesc
	FRHITextureCreateDesc(
		FRHITextureDesc const& InDesc
		, ERHIAccess InInitialState
		, TCHAR const* InDebugName
		, FResourceBulkDataInterface* InBulkData = nullptr
	)
		: FRHITextureDesc(InDesc)
		, InitialState(InInitialState)
		, DebugName(InDebugName)
		, BulkData(InBulkData)
	{
	}

	/*void CheckValidity() const
	{
		FRHITextureDesc::CheckValidity(*this, DebugName);

		ensureMsgf(InitialState != ERHIAccess::Unknown, TEXT("Resource %s cannot be created in an unknown state."), DebugName);
	}*/

	FRHITextureCreateDesc& SetFlags(ETextureCreateFlags InFlags) { Flags = InFlags;                          return *this; }
	FRHITextureCreateDesc& AddFlags(ETextureCreateFlags InFlags) { Flags |= InFlags;                         return *this; }
	FRHITextureCreateDesc& SetClearValue(FClearValueBinding InClearValue) { ClearValue = InClearValue;                return *this; }
	FRHITextureCreateDesc& SetExtData(uint32 InExtData) { ExtData = InExtData;                      return *this; }
	FRHITextureCreateDesc& SetExtent(const FVector3D& InExtent) { Extent = InExtent;                        return *this; }
	FRHITextureCreateDesc& SetExtent(int32 InExtentX, int32 InExtentY) { Extent = FVector3D(InExtentX, InExtentY, 0); return *this; }
	FRHITextureCreateDesc& SetExtent(uint32 InExtent) { Extent = FVector3D(InExtent, InExtent, 0);             return *this; }
	FRHITextureCreateDesc& SetDepth(uint16 InDepth) { Depth = InDepth;                          return *this; }
	FRHITextureCreateDesc& SetArraySize(uint16 InArraySize) { ArraySize = InArraySize;                  return *this; }
	FRHITextureCreateDesc& SetNumMips(uint8 InNumMips) { NumMips = InNumMips;                      return *this; }
	FRHITextureCreateDesc& SetNumSamples(uint8 InNumSamples) { NumSamples = InNumSamples;                return *this; }
	FRHITextureCreateDesc& SetDimension(ETextureDimension InDimension) { Dimension = InDimension;                  return *this; }
	FRHITextureCreateDesc& SetFormat(EPixelFormat InFormat) { Format = InFormat;                        return *this; }
	FRHITextureCreateDesc& SetUAVFormat(EPixelFormat InUAVFormat) { UAVFormat = InUAVFormat;                  return *this; }
	FRHITextureCreateDesc& SetInitialState(ERHIAccess InInitialState) { InitialState = InInitialState;            return *this; }
	FRHITextureCreateDesc& SetDebugName(const TCHAR* InDebugName) { DebugName = InDebugName;                  return *this; }
	//FRHITextureCreateDesc& SetGPUMask(FRHIGPUMask InGPUMask) { GPUMask = InGPUMask;                      return *this; }
	FRHITextureCreateDesc& SetBulkData(FResourceBulkDataInterface* InBulkData) { BulkData = InBulkData;                    return *this; }
	FRHITextureCreateDesc& DetermineInititialState() { if (InitialState == ERHIAccess::Unknown) InitialState = RHIGetDefaultResourceState(Flags, BulkData != nullptr); return *this; }
	FRHITextureCreateDesc& SetFastVRAMPercentage(float In) { FastVRAMPercentage = uint8(FMath::Clamp(In, 0.f, 1.0f) * 0xFF); return *this; }
	FRHITextureCreateDesc& SetClassName(const FName& InClassName) { ClassName = InClassName;				   return *this; }
	FRHITextureCreateDesc& SetOwnerName(const FName& InOwnerName) { OwnerName = InOwnerName;                  return *this; }
	//FName GetTraceClassName() const { const static FLazyName FRHITextureName(TEXT("FRHITexture")); return (ClassName == NAME_None) ? FRHITextureName : ClassName; }

	/* The RHI access state that the resource will be created in. */
	ERHIAccess InitialState = ERHIAccess::Unknown;

	/* A friendly name for the resource. */
	const TCHAR* DebugName = nullptr;

	/* Optional initial data to fill the resource with. */
	FResourceBulkDataInterface* BulkData = nullptr;

	FName ClassName = NAME_None;	// The owner class of FRHITexture used for Insight asset metadata tracing
	FName OwnerName = NAME_None;	// The owner name used for Insight asset metadata tracing
};

/** The base type of RHI resources. */
class FRHIResource
{
public:
	RHI_API FRHIResource(ERHIResourceType InResourceType);
	RHI_API virtual ~FRHIResource();

	inline uint32 AddRef() const
	{
		int32 NewNum = AtomicFlags.AddRef(std::memory_order_acquire);
		_ASSERT(NewNum > 0);
		return uint32(NewNum);
	}

private:
	// 강제 인라이닝을 방지하기 위한 별도의 함수입니다. 이는 코드 크기와 성능 모두에 도움이 됩니다.
	RHI_API void Destroy() const;

public:
	inline uint32 Release() const
	{
		int32 NewNum = AtomicFlags.Release(std::memory_order_release);
		_ASSERT(NewNum >= 0);

		if (NewNum == 0)
		{
			Destroy();
		}
		_ASSERT(NewNum >= 0);
		return uint32(NewNum);
	}

	inline uint32 GetRefCount() const
	{
		int32 CurrentNum = AtomicFlags.GetNumRefs(std::memory_order_relaxed);
		_ASSERT(CurrentNum >= 0);
		return uint32(CurrentNum);
	}

	bool IsValid() const
	{
		return AtomicFlags.IsValid(std::memory_order_relaxed);
	}

	void Delete()
	{
		AtomicFlags.MarkForDelete(std::memory_order_acquire);
		CurrentlyDeleting = this;
		delete this;
	}

	void DisableLifetimeExtension()
	{
		_ASSERT(IsValid(), TEXT("Resource is already marked for deletion. This call is a no-op. DisableLifetimeExtension must be called while still holding a live reference."));
		bAllowExtendLifetime = false;
	}

	inline ERHIResourceType GetType() const { return ResourceType; }

private:
	class FAtomicFlags
	{
		static constexpr ::uint32 MarkedForDeleteBit = 1 << 30;
		static constexpr ::uint32 DeletingBit = 1 << 31;
		static constexpr ::uint32 NumRefsMask = ~(MarkedForDeleteBit | DeletingBit);

		std::atomic_uint Packed = { 0 };

	public:
		int32 AddRef(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_add(1, MemoryOrder);
			_ASSERT((OldPacked & DeletingBit) == 0, TEXT("Resource is being deleted."));
			int32  NumRefs = (OldPacked & NumRefsMask) + 1;
			_ASSERT(NumRefs < NumRefsMask, TEXT("Reference count has overflowed."));
			return NumRefs;
		}

		int32 Release(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_sub(1, MemoryOrder);
			_ASSERT((OldPacked & DeletingBit) == 0, TEXT("Resource is being deleted."));
			int32  NumRefs = (OldPacked & NumRefsMask) - 1;
			_ASSERT(NumRefs >= 0, TEXT("Reference count has underflowed."));
			return NumRefs;
		}

		bool MarkForDelete(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_or(MarkedForDeleteBit, MemoryOrder);
			_ASSERT((OldPacked & DeletingBit) == 0);
			return (OldPacked & MarkedForDeleteBit) != 0;
		}

		bool UnmarkForDelete(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_xor(MarkedForDeleteBit, MemoryOrder);
			_ASSERT((OldPacked & DeletingBit) == 0);
			bool  OldMarkedForDelete = (OldPacked & MarkedForDeleteBit) != 0;
			_ASSERT(OldMarkedForDelete == true);
			return OldMarkedForDelete;
		}

		bool Deleteing()
		{
			uint32 LocalPacked = Packed.load(std::memory_order_acquire);
			_ASSERT((LocalPacked & MarkedForDeleteBit) != 0);
			_ASSERT((LocalPacked & DeletingBit) == 0);
			uint32 NumRefs = LocalPacked & NumRefsMask;

			if (NumRefs == 0) // caches can bring dead objects back to life
			{
				//#if DO_CHECK
				//				Packed.fetch_or(DeletingBit, std::memory_order_acquire);
				//#endif
				return true;
			}
			else
			{
				UnmarkForDelete(std::memory_order_release);
				return false;
			}
		}

		bool IsValid(std::memory_order MemoryOrder)
		{
			uint32 LocalPacked = Packed.load(MemoryOrder);
			return (LocalPacked & MarkedForDeleteBit) == 0 && (LocalPacked & NumRefsMask) != 0;
		}

		bool IsMarkedForDelete(std::memory_order MemoryOrder)
		{
			return (Packed.load(MemoryOrder) & MarkedForDeleteBit) != 0;
		}

		int32 GetNumRefs(std::memory_order MemoryOrder)
		{
			return Packed.load(MemoryOrder) & NumRefsMask;
		}
	};
	mutable FAtomicFlags AtomicFlags;

	const ERHIResourceType ResourceType;
	uint8 bCommitted : 1;
	uint8 bAllowExtendLifetime : 1;

	RHI_API static FRHIResource* CurrentlyDeleting;

	friend FRHICommandList;
};

class FRHIViewport : public FRHIResource
{
public:
	FRHIViewport() : FRHIResource(RRT_Viewport) {}
};