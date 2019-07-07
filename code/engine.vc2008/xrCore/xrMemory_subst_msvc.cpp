#include "stdafx.h"
#include "xrMemory.h"
#include "xrMemoryDebug.h"
#include "../FRayBuildConfig.hpp"
#include "mimalloc/mimalloc.h"

#ifdef TBB_ALLOC
	#include "tbb/scalable_allocator.h"
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
	stat_calls++;

	void* ptr = nullptr;
#ifdef TBB_ALLOC
	ptr = scalable_malloc(size);
#else
	if constexpr (MEM_HARD_DEBUG)
	{
		ptr = DebugAllocate(size, dwPageSize);
	}
	else if constexpr(MEM_PURE_ALLOC)
	{
		ptr = HeapAlloc(hHeap, 0, size);
	}
	else
	{
		ptr = mi_malloc_aligned(size, 16);
	}
#endif

#if defined(DEBUG) && defined(MEM_DEBUG)
	RegisterPointer(ptr);
#endif

	return ptr;
}

void xrMemory::mem_free(void* P)
{
	stat_calls++;
#if defined(DEBUG) && defined(MEM_DEBUG)
	UnregisterPointer(P);
#endif

#ifdef TBB_ALLOC
	scalable_free(P);
#else
	if constexpr (MEM_HARD_DEBUG)
	{
		VirtualFree(P, 0, MEM_RELEASE);
	}
	else if constexpr(MEM_PURE_ALLOC)
	{
		HeapFree(hHeap, 0, P);
	}
	else
	{
		mi_free(P);
	}
#endif

}

void* xrMemory::mem_realloc(void* P, size_t size)
{
#if defined(DEBUG) && defined(MEM_DEBUG)
	UnregisterPointer(P);
#endif

	stat_calls++;

	void* ptr = nullptr;
#ifdef TBB_ALLOC
	ptr = scalable_realloc(P, size);
#else
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
	else
	{
		ptr = mi_realloc_aligned(P, size, 16);
	}
#endif

#if defined(DEBUG) && defined(MEM_DEBUG)
	RegisterPointer(ptr);
#endif

	return ptr;
}

void xrMemory::debug_MarkPointerAsChoosenOne(void* ptr)
{
#if defined(DEBUG) && defined(MEM_DEBUG)
	MarkPointerAsChoosenOne(ptr);
#endif
}
