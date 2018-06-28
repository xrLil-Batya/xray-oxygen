#include "stdafx.h"
#include "xrMemory.h"
#include "xrMemoryDebug.h"

//#define MEM_DEBUG

void* xrMemory::mem_alloc(size_t size)
{
	stat_calls++;
	void* ptr = malloc(size);

#ifdef defined (DEBUG) && defined (MEM_DEBUG)
    RegisterPointer(ptr);
#endif

    return ptr;
}

void xrMemory::mem_free(void* P)
{
	stat_calls++;
#ifdef defined (DEBUG) && defined (MEM_DEBUG)
    UnregisterPointer(P);
#endif
	free(P);
}

void* xrMemory::mem_realloc(void* P, size_t size)
{
#ifdef defined(DEBUG) && defined(MEM_DEBUG)
    UnregisterPointer(P);
#endif

	stat_calls++;
    void* ptr = realloc(P, size);

#ifdef defined(DEBUG) && defined(MEM_DEBUG)
    RegisterPointer(ptr);
#endif

    return ptr;
}