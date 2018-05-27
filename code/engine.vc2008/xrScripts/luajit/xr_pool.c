// Author:	Abramcumner
// Modifer: ForserX, Giperion

#include "lj_def.h"
#include "lj_arch.h"
#include "../../xrCore/xrMemory_C.h"
#include "../../FrayBuildConfig.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "xr_pool.h"

#define MAX_SIZE_T		(~(size_t)0)
#define MFAIL			((void *)(MAX_SIZE_T))

#define CHUNK_SIZE (128 * 1024)
#define CHUNK_COUNT 1024

#include <oxy\cboolean.h>
static bool inited = false;
static DWORD refCounter = 0;

void* g_heap;
size_t g_heap_size = CHUNK_SIZE * CHUNK_COUNT;
char g_heapMap[CHUNK_COUNT + 1];
char* g_firstFreeChunk;
char* find_free(int size);

#ifdef JIT_OXY_MEMPOOL_DEBUG
static char buf[100];
void dump_map(void* ptr, size_t size, char c);
#endif

LPVOID possibleAddressesTable[] =
{
    0x11000000,
    0x22000000,
    0x33000000,
    0x44000000,
    0x55000000,
    0x66000000,
    0x77000000,
    0xAA000000,
    0xBB000000,
    0xCC000000,
    0xDD000000,
    0xEE000000,
    0xFF000000,
};

BOOL XR_INIT()
{
	if (!inited)
	{
		g_heap = NULL;
// 		long st = ntavm(INVALID_HANDLE_VALUE, &g_heap, NTAVM_ZEROBITS, &size,
// 			MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        //g_heap = xr_malloc_C(size)

        for (int i = 0; i < (sizeof(possibleAddressesTable) / sizeof(LPVOID)); ++i)
        {
            if (g_heap != NULL)
            {
                VirtualFree(g_heap, 0, MEM_RELEASE);
                g_heap = NULL;
            }

            g_heap = VirtualAlloc(possibleAddressesTable[i], g_heap_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

            if (g_heap < (LPVOID)0x00000000FFFFFFFFu) break;
        }

        if (g_heap > (LPVOID)0x00000000FFFFFFFFu)
        {
            DebugBreak();
        }

        if (g_heap == NULL) return FALSE;
		for (unsigned i = 0; i < CHUNK_COUNT; i++)
		{
			g_heapMap[i] = 'x';
		}

		g_heapMap[CHUNK_COUNT] = '\0';
		g_firstFreeChunk = g_heapMap;

#ifdef JIT_OXY_MEMPOOL_DEBUG	
		sprintf(buf, "XR_INIT create_block %p result=%X\r\n", g_heap, st);
		OutputDebugString(buf);
#endif
		inited = true;
	}

    ++refCounter;
    return TRUE;
}


void XR_DESTROYPOOL()
{
    if (--refCounter == 0)
    {
        VirtualFree(g_heap, 0, MEM_RELEASE);
    }
}

void* XR_MMAP(size_t size)
{
#ifdef JIT_OXY_MEMPOOL_DEBUG
	sprintf(buf, "XR_MMAP(%Iu)", size);
	OutputDebugString(buf);
#endif
	int chunks = (int)size / CHUNK_SIZE;
	char* s = find_free(chunks);
	void* ptr = MFAIL;
	if (s != NULL) {
		ptr = (char*)g_heap + CHUNK_SIZE * (s - g_heapMap);
		for (int i = 0; i < chunks; i++)
			s[i] = 'a' + chunks - 1;
		if (s == g_firstFreeChunk)
			g_firstFreeChunk = find_free(1);
	}
#ifdef JIT_OXY_MEMPOOL_DEBUG
	sprintf(buf, " ptr=%p chunks %d\r\n", ptr, chunks);
	OutputDebugString(buf);
	dump_map(ptr, size, 'U');
#endif
	return ptr;
}

void XR_DESTROY(void* ptr, size_t size)
{
	if(ptr != 0) // FX: Sometimes the debugger crashes on ptr
	{
#ifdef JIT_OXY_MEMPOOL_DEBUG
		sprintf(buf, "XR_DESTROY(ptr=%p, size=%Iu)", ptr, size);
		OutputDebugString(buf);
#endif
		char* s = g_heapMap + ((char*)ptr - (char*)g_heap) / CHUNK_SIZE;
		int count = (int)size / CHUNK_SIZE;
		for (int i = 0; i < count; i++)
			s[i] = 'x';
		if (s < g_firstFreeChunk)
			g_firstFreeChunk = s;
#ifdef JIT_OXY_MEMPOOL_DEBUG	
		dump_map(ptr, size, 'X');
#endif
	}
}

char* find_free(int size)
{
	char* p = g_firstFreeChunk;
	int count = 0;
	while (*p != '\0')
	{
		if (*p == 'x')
			count++;
		else
			count = 0;
		p++;
		if (count == size)
			return p - count;
	}
	return NULL;
}

#ifdef JIT_OXY_MEMPOOL_DEBUG
static 	char temp[1025];
void dump_map(void* ptr, size_t size, char c)
{
	strcpy(temp, g_heapMap);
	char *cur = temp + ((char*)ptr - (char*)g_heap) / CHUNK_SIZE;

	for (unsigned i = 0; i < size / CHUNK_SIZE; i++)
	{
		cur[i] = c;
	}

	for (unsigned i = 0; i < 8; i++)
	{
		char a = temp[i * 128 + 128];
		temp[i * 128 + 128] = '\0';
		OutputDebugString(temp + i * 128);
		temp[i * 128 + 128] = a;
		OutputDebugString("\r\n");
	}
}
#endif