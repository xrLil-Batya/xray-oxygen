#include "stdafx.h"
#include "xrsharedmem.h"
#include "FrayBuildConfig.hpp"
#include "mimalloc/mimalloc.h"

xrMemory Memory;

xrMemory::xrMemory()
{
	// do not place code here. 
	// If you need something initialized - place that in _initialize
}

xrMemory::~xrMemory()
{}

void xrMemory::_initialize()
{
	hHeap = HeapCreate(0, 0, 0);
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	dwPageSize = info.dwPageSize;
	bInitialized = true;
}

void xrMemory::_destroy()
{
	g_pSharedMemoryContainer_isDestroyed = true;
	xr_delete(g_pSharedMemoryContainer);
	g_pSharedMemoryContainer = nullptr;
	bInitialized = false;
}

inline const size_t external_size = size_t(-1);

void xrMemory::mem_compact()
{
	_heapmin();

	HeapCompact(GetProcessHeap(), 0);

	g_pStringContainer.clean();

	if (g_pSharedMemoryContainer)
		g_pSharedMemoryContainer->clean();

	if (strstr(Core.Params, "-swap_on_compact"))
		SetProcessWorkingSetSize(GetCurrentProcess(), external_size, external_size);
}

// xr_strdup
char* xr_strdup(const char* string)
{
	if constexpr (MEM_PURE_ALLOC)
	{
		VERIFY(string);
		size_t len = xr_strlen(string) + 1;
		char *	memory = (char*)Memory.mem_alloc(len);
		std::memcpy(memory, string, len);

		return memory;
	}
	else
	{
		return mi_strdup(string);
	}
}

XRCORE_API bool is_stack_ptr(void* _ptr)
{
	int local_value = 0;
	void* ptr_refsound = _ptr;
	void* ptr_local = &local_value;
	ptrdiff_t difference = (ptrdiff_t)_abs(s64(ptrdiff_t(ptr_local) - ptrdiff_t(ptr_refsound)));
	return (difference < (512 * 1024));
}

extern "C"
{
    XRCORE_API void* xr_malloc_C(size_t size)
    {
        return Memory.mem_alloc(size);
    }

    XRCORE_API void	xr_free_C(void* ptr)
    {
        Memory.mem_free(ptr);
    }

    XRCORE_API void* xr_realloc_C(void* ptr, size_t size)
    {
        return Memory.mem_realloc(ptr, size);
    }
}