#include "stdafx.h"
#pragma hdrstop

#include	"xrsharedmem.h"
#include	<malloc.h>

xrMemory Memory;
bool mem_initialized = false;

void xrMemory::_initialize()
{
	stat_calls = 0;
	stat_counter = 0;

	mem_initialized = true;
}

void xrMemory::_destroy()
{
	xr_delete(g_pSharedMemoryContainer);

	mem_initialized = false;
}

inline const size_t external_size = size_t(-1);

void xrMemory::mem_compact()
{
	RegFlushKey(HKEY_CLASSES_ROOT);
	RegFlushKey(HKEY_CURRENT_USER);

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
	VERIFY(string);
	size_t len = xr_strlen(string) + 1;
	char *	memory = (char*)Memory.mem_alloc(len);
	std::memcpy(memory, string, len);

	return memory;
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
    XRCORE_API void*	xr_malloc_C(size_t size)
    {
        return Memory.mem_alloc(size);
    }

    XRCORE_API void	xr_free_C(void* ptr)
    {
        Memory.mem_free(ptr);
    }

    XRCORE_API void*	xr_realloc_C(void* ptr, size_t size)
    {
        return Memory.mem_realloc(ptr, size);
    }
}