#include "stdafx.h"
#include "xrMemory.h"
#include "xrMemoryDebug.h"
#include "../FRayBuildConfig.hpp"

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
	else
	{
		ptr = HeapAlloc(hHeap, 0, size);
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
	else
	{
		HeapFree(hHeap, 0, P);
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
		VirtualFree(P, 0, MEM_RELEASE);
		ptr = DebugAllocate(size, dwPageSize);
	}
	else
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
