#include "stdafx.h"
#include "xrMemory.h"
#include "xrMemoryDebug.h"
#include "../FRayBuildConfig.hpp"
#include "mimalloc/mimalloc.h"
#include "tbb/scalable_allocator.h"

#ifdef TBB_ALLOC
#define USED_TBB_ALLOC 1
#else
#define USED_TBB_ALLOC 0
#endif


char* DebugAllocate(size_t size, DWORD dwPageSize)
{
	char* memory = (char*)VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	return memory;
}

void* xrMemory::mem_alloc(size_t size)
{
	if (!bInitialized)
	{
		_initialize();
	}
	Profiling.StartAlloc(size);
	stat_calls++;

	void* ptr = nullptr;

	if constexpr (MEM_HARD_DEBUG)
	{
		ptr = DebugAllocate(size, dwPageSize);
	}
	else if constexpr(MEM_PURE_ALLOC)
	{
		ptr = HeapAlloc(hHeap, 0, size);
	}
	else if constexpr (USED_TBB_ALLOC)
	{
		ptr = scalable_malloc(size);
	}
	else if constexpr(MEM_MIMALLOC)
	{
		ptr = mi_malloc_aligned(size, 16);
	}

#if defined(DEBUG) && defined(MEM_DEBUG)
	RegisterPointer(ptr);
#endif

	if constexpr (MEM_SUPER_HARD_DEBUG)
	{
		WriteAllocation(ptr, size);
	}

	Profiling.EndAlloc(ptr, size);
	return ptr;
}

void* xrMemory::mem_alloc(size_t size, size_t aligment)
{
	if (!bInitialized)
	{
		_initialize();
	}
	Profiling.StartAlloc(size);
	stat_calls++;

	void* ptr = nullptr;
	if constexpr (MEM_HARD_DEBUG)
	{
		ptr = DebugAllocate(size, dwPageSize);
	}
	else if constexpr (MEM_PURE_ALLOC)
	{
		// no version with aligment option
		ptr = HeapAlloc(hHeap, 0, size);
	}
	else if constexpr (USED_TBB_ALLOC)
	{
		ptr = scalable_aligned_malloc(size, aligment);
	}
	else if constexpr (MEM_MIMALLOC)
	{
		ptr = mi_malloc_aligned(size, aligment);
	}

#if defined(DEBUG) && defined(MEM_DEBUG)
	RegisterPointer(ptr);
#endif

	if constexpr (MEM_SUPER_HARD_DEBUG)
	{
		WriteAllocation(ptr, size);
	}

	Profiling.EndAlloc(ptr, size);
	return ptr;
}


void xrMemory::mem_free(void* P)
{
	if (P == nullptr)
	{
		return;
	}
	Profiling.StartFree(P);
	stat_calls++;
#if defined(DEBUG) && defined(MEM_DEBUG)
	UnregisterPointer(P);
#endif

	if constexpr (MEM_HARD_DEBUG)
	{
		VirtualFree(P, 0, MEM_RELEASE);
	}
	else if constexpr(MEM_PURE_ALLOC)
	{
		HeapFree(hHeap, 0, P);
	}
	else if constexpr (USED_TBB_ALLOC)
	{
		scalable_free(P);
	}
	else if constexpr (MEM_MIMALLOC)
	{
		mi_free(P);
	}

	if constexpr (MEM_SUPER_HARD_DEBUG)
	{
		WriteFree(P);
	}

	Profiling.EndFree(P);
}

void* xrMemory::mem_realloc(void* P, size_t size)
{
	Profiling.StartReAlloc(P, size);
#if defined(DEBUG) && defined(MEM_DEBUG)
	UnregisterPointer(P);
#endif

	stat_calls++;

	void* ptr = nullptr;
	if constexpr (MEM_HARD_DEBUG)
	{
		ptr = DebugAllocate(size, dwPageSize);
		if (P != nullptr)
		{
			MEMORY_BASIC_INFORMATION info;
			VirtualQuery(P, &info, sizeof(info));
			size_t copiedSize = std::min(size, info.RegionSize);
			memcpy(ptr, P, copiedSize);
			VirtualFree(P, 0, MEM_RELEASE);
		}
	}
	else if constexpr(MEM_PURE_ALLOC)
	{
		if (P == nullptr)
		{
			ptr = HeapAlloc(hHeap, 0, size);
		}
		else
		{
			ptr = HeapReAlloc(hHeap, 0, P, size);
		}
	}
	else if constexpr (USED_TBB_ALLOC)
	{
		ptr = scalable_realloc(P, size);
	}
	else if constexpr (MEM_MIMALLOC)
	{
		ptr = mi_realloc_aligned(P, size, 16);
	}

#if defined(DEBUG) && defined(MEM_DEBUG)
	RegisterPointer(ptr);
#endif

	if constexpr (MEM_SUPER_HARD_DEBUG)
	{
		if (P != ptr) // previous pointer was freed
		{
			if (P != nullptr)
			{
				WriteFree(P);
			}
			WriteAllocation(ptr, size);
		}
		else // pointer is same, but size if different now
		{
			WriteReAllocation(P, size);
		}
	}

	Profiling.EndReAlloc(P, &ptr, size);
	return ptr;
}

void xrMemory::debug_MarkPointerAsChoosenOne(void* ptr)
{
#if defined(DEBUG) && defined(MEM_DEBUG)
	MarkPointerAsChoosenOne(ptr);
#endif
}
