#pragma once
#include "CoreMinimal.h"

enum class EGpuVendorId : uint32
{
    Unknown = 0xffffffff,
    NotQueried = 0,

    Amd = 0x1002,
    ImgTec = 0x1010,
    Nvidia = 0x10DE,
    Arm = 0x13B5,
    Broadcom = 0x14E4,
    Qualcomm = 0x5143,
    Intel = 0x8086,
    Apple = 0x106B,
    Vivante = 0x7a05,
    VeriSilicon = 0x1EB1,
    SamsungAMD = 0x144D,
    Microsoft = 0x1414,

    Kazan = 0x10003,    // VkVendorId
    Codeplay = 0x10004,    // VkVendorId
    Mesa = 0x10005,    // VkVendorId
};

inline EGpuVendorId RHIConvertToGpuVendorId(uint32 VendorId)
{
    switch ((EGpuVendorId)VendorId)
    {
    case EGpuVendorId::NotQueried:
        return EGpuVendorId::NotQueried;

    case EGpuVendorId::Amd:
    case EGpuVendorId::Mesa:
    case EGpuVendorId::ImgTec:
    case EGpuVendorId::Nvidia:
    case EGpuVendorId::Arm:
    case EGpuVendorId::Broadcom:
    case EGpuVendorId::Qualcomm:
    case EGpuVendorId::Intel:
    case EGpuVendorId::SamsungAMD:
    case EGpuVendorId::Microsoft:
        return (EGpuVendorId)VendorId;

    default:
        break;
    }

    return EGpuVendorId::Unknown;
}


/** An enumeration of the different RHI reference types. */
enum ERHIResourceType : uint8
{
	RRT_None,

	RRT_SamplerState,
	RRT_RasterizerState,
	RRT_DepthStencilState,
	RRT_BlendState,
	RRT_VertexDeclaration,
	RRT_VertexShader,
	RRT_MeshShader,
	RRT_AmplificationShader,
	RRT_PixelShader,
	RRT_GeometryShader,
	RRT_RayTracingShader,
	RRT_ComputeShader,
	RRT_GraphicsPipelineState,
	RRT_ComputePipelineState,
	RRT_RayTracingPipelineState,
	RRT_BoundShaderState,
	RRT_UniformBufferLayout,
	RRT_UniformBuffer,
	RRT_Buffer,
	RRT_Texture,
	// @todo: texture type unification - remove these
	RRT_Texture2D,
	RRT_Texture2DArray,
	RRT_Texture3D,
	RRT_TextureCube,
	// @todo: texture type unification - remove these
	RRT_TextureReference,
	RRT_TimestampCalibrationQuery,
	RRT_GPUFence,
	RRT_RenderQuery,
	RRT_RenderQueryPool,
	RRT_Viewport,
	RRT_UnorderedAccessView,
	RRT_ShaderResourceView,
	RRT_RayTracingAccelerationStructure,
	RRT_StagingBuffer,
	RRT_CustomPresent,
	RRT_ShaderLibrary,
	RRT_PipelineBinaryLibrary,
	RRT_ShaderBundle,

	RRT_Num
};