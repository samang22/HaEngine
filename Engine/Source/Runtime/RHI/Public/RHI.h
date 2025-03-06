#pragma once
#include "DynamicRHI.h"
#include "RHIDefinitions.h"
#include "RHIGlobals.h"

/** Initializes the RHI. */
extern RHI_API void RHIInit();
extern RHI_API void RHIExit();

struct FVertexElement
{
	uint8 StreamIndex;
	uint8 Offset;
	EVertexElementType Type;
	uint8 AttributeIndex;
	uint16 Stride;
	///**
	// * 요소를 소비하기 위해 인스턴스 인덱스나 버텍스 인덱스를 사용할지 여부를 결정합니다.
	// * 예를 들어, bUseInstanceIndex가 0이면, 요소가 모든 인스턴스에 대해 반복됩니다.
	// */
	//uint16 bUseInstanceIndex;

	FVertexElement() {}
	FVertexElement(uint8 InStreamIndex, uint8 InOffset, EVertexElementType InType, uint8 InAttributeIndex, uint16 InStride/*, bool bInUseInstanceIndex = false*/) :
		StreamIndex(InStreamIndex),
		Offset(InOffset),
		Type(InType),
		AttributeIndex(InAttributeIndex),
		Stride(InStride)//,
		//bUseInstanceIndex(bInUseInstanceIndex)
	{
	}

	bool operator==(const FVertexElement& Other) const
	{
		return (StreamIndex == Other.StreamIndex &&
			Offset == Other.Offset &&
			Type == Other.Type &&
			AttributeIndex == Other.AttributeIndex &&
			Stride == Other.Stride);//&&
		//bUseInstanceIndex == Other.bUseInstanceIndex);
	}

	/*friend FArchive& operator<<(FArchive& Ar, FVertexElement& Element)
	{
		Ar << Element.StreamIndex;
		Ar << Element.Offset;
		Ar << Element.Type;
		Ar << Element.AttributeIndex;
		Ar << Element.Stride;
		Ar << Element.bUseInstanceIndex;
		return Ar;
	}*/
	//RHI_API FString ToString() const;
	//RHI_API void FromString(const FString& Src);
	//RHI_API void FromString(const FStringView& Src);
};

struct FRasterizerStateInitializerRHI
{
	ERasterizerFillMode FillMode = FM_Point;
	ERasterizerCullMode CullMode = CM_None;
	float DepthBias = 0.0f;
	float SlopeScaleDepthBias = 0.0f;
	ERasterizerDepthClipMode DepthClipMode = ERasterizerDepthClipMode::DepthClip;
	bool bAllowMSAA = false;

	FRasterizerStateInitializerRHI() = default;
	FRasterizerStateInitializerRHI(const FRasterizerStateInitializerRHI&) = default;
	FRasterizerStateInitializerRHI(FRasterizerStateInitializerRHI&&) = default;
	FRasterizerStateInitializerRHI& operator=(const FRasterizerStateInitializerRHI&) = default;

	FRasterizerStateInitializerRHI(ERasterizerFillMode InFillMode, ERasterizerCullMode InCullMode, bool bInAllowMSAA)
		: FillMode(InFillMode)
		, CullMode(InCullMode)
		, bAllowMSAA(bInAllowMSAA)
	{
	}

	FRasterizerStateInitializerRHI(ERasterizerFillMode InFillMode, ERasterizerCullMode InCullMode, float InDepthBias, float InSlopeScaleDepthBias, ERasterizerDepthClipMode InDepthClipMode, bool bInAllowMSAA)
		: FillMode(InFillMode)
		, CullMode(InCullMode)
		, DepthBias(InDepthBias)
		, SlopeScaleDepthBias(InSlopeScaleDepthBias)
		, DepthClipMode(InDepthClipMode)
		, bAllowMSAA(bInAllowMSAA)
	{
	}


	//RHI_API friend uint32 GetTypeHash(const FRasterizerStateInitializerRHI& Initializer);
	RHI_API friend bool operator== (const FRasterizerStateInitializerRHI& A, const FRasterizerStateInitializerRHI& B);
};