#pragma once
#include "CoreTypes.h"
#include "Templates/RefCounting.h"
#include "RHIDefinitions.h"
#include "RHICommandList.h"

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