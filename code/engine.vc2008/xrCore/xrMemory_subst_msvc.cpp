#include "stdafx.h"
#include "xrMemory.h"
#include "xrMemoryDebug.h"
#include "../FRayBuildConfig.hpp"

void* xrMemory::mem_alloc(size_t size)
{
	stat_calls++;
	void* ptr = malloc(size);

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
	free(P);
}

void* xrMemory::mem_realloc(void* P, size_t size)
{
#if defined(DEBUG) && defined(MEM_DEBUG)
	UnregisterPointer(P);
#endif

	stat_calls++;
	void* ptr = realloc(P, size);

#if defined(DEBUG) && defined(MEM_DEBUG)
	RegisterPointer(ptr);
#endif

	return ptr;
}