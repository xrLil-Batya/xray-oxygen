#pragma once

XRCORE_API u32 mem_usage_impl(u32* pBlocksUsed = nullptr, u32* pBlocksFree = nullptr);

static const size_t AllocationHistorySize = 512 * 1024 * 1024; // 512 MB

class XRCORE_API xrMemory
{
public:
	xrMemory();
	~xrMemory();
	void				_initialize();
	void				_destroy();

	u32					stat_calls = 0;
public:
	u32					mem_usage();
	void				mem_compact();

	void*				mem_alloc(size_t size);
	void*				mem_alloc(size_t size, size_t aligment);

	void*				mem_realloc(void* p, size_t size);
	void				mem_free(void* p);

	void				PrintVersion();
	void				PrintStat();

	void				debug_MarkPointerAsChoosenOne(void* ptr);

	// should be invoked only once per application life.
	void				PrintAllPointerHistory(void* ptr);

private:

	enum class AllocRecordStatus : u32
	{
		Allocated,
		Freed,
		Realloc
	};

	struct AllocRecord
	{
		void* ptr;					//  8
		void* frame;				//  16
		size_t size;				//  24
		AllocRecordStatus AllocFlag;//  28
		time_t time;				//  36
		DWORD threadID;				//  40
	}; // 40 total

	HANDLE hHeap = NULL;
	DWORD dwPageSize = 0;
	bool bInitialized = false;

	void initializeHardDebug();
	void WriteAllocation(void* Ptr, size_t size);
	void WriteReAllocation(void* Ptr, size_t size);
	void WriteFree(void* Ptr);

	void PrepareAllocEvent(void* Ptr, size_t size, AllocRecordStatus status);
	void WriteAllocRecord(AllocRecord& record);
	void FlushAllocEvents();
	HANDLE hDebugHeap = NULL;
	char* AllocationHistory = nullptr;
	size_t AllocationHistoryWriteCursor = 0;
	FILE* hAllocHistoryFile = NULL;
	// do not use xrCriticalSection because of early initialization
	CRITICAL_SECTION allocEventGuard;

};

extern XRCORE_API xrMemory Memory;

// Required to be here, before xr_malloc
#include "xrMemoryUtils.h"
// delete
#include "xrMemory_subst_msvc.h"

// generic "C"-like allocations/deallocations
template <class T>
IC T*		xr_alloc		(size_t count)			{ return (T*)Memory.mem_alloc(count * sizeof(T)); }

template <class T>
IC void		xr_free			(T* &P)					{ if (P) { Memory.mem_free((void*)P); P = nullptr;}; }

template<class T>
IC void xr_free(const xrScopePtr<T>& mem)
{
	static_assert (false, "Memory inside xrScopedPtr should NOT be freed manually");
}

IC void*	xr_malloc		(size_t size)			{ return Memory.mem_alloc(size); }
IC void*	xr_realloc		(void* P, size_t size)	{ return Memory.mem_realloc(P, size); }

XRCORE_API char* xr_strdup(const char* string);
#if !defined( _CLR_MANAGER) && !defined(GM_NON_GAME)
#pragma warning(push)
#pragma warning( disable : 4595)  
IC void*	operator new		(size_t size)	{ return Memory.mem_alloc(size ? size : 1); }
IC void*	operator new[]		(size_t size)	{ return Memory.mem_alloc(size ? size : 1); }
IC void		operator delete		(void *p)		{ xr_free(p); }
IC void		operator delete[]	(void* p)		{ xr_free(p); }
#pragma warning(pop)
#endif
// POOL-ing
const		u32			mem_pools_count = 54;
const		u32			mem_pools_ebase = 16;
const		u32			mem_generic = mem_pools_count + 1;

XRCORE_API void vminfo(size_t *_free, size_t *reserved, size_t *committed);
XRCORE_API void log_vminfo();

