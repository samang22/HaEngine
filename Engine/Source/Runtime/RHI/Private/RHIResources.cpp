#include "RHIResources.h"

FRHIResource::FRHIResource(ERHIResourceType InResourceType)
	: ResourceType(InResourceType)
{
}

FRHIResource::~FRHIResource()
{
}

RHI_API void FRHIResource::Destroy() const
{
	delete this;
}
