#include "stdafx.h"
#include "xrMemory.h"
#include "xrMemoryDebug.h"
#include "../FRayBuildConfig.hpp"

#ifdef TBB_ALLOC
	#include "tbb/scalable_allocator.h"
#endif

void* xrMemory::mem_alloc(size_t size)
{
	stat_calls++;

	void* ptr = nullptr;
#ifdef TBB_ALLOC
	ptr = scalable_malloc(size);
#else
	ptr = malloc(size);
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
	free(P);
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
	ptr = realloc(P, size);
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
