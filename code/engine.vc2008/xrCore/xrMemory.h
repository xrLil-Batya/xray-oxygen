#pragma once
#define DUMP_PHASE	do {} while (0)

XRCORE_API u32 mem_usage_impl(u32* pBlocksUsed = nullptr, u32* pBlocksFree = nullptr);

class XRCORE_API xrMemory
{
public:
	struct mdbg
	{
		void*			_p;
		size_t 			_size;
		const char*		_name;
		u32				_dummy;
	};
public:
	xrMemory()			= default;
	void				_initialize();
	void				_destroy();

	u32					stat_calls;
	s32					stat_counter;
public:
	IC u32				mem_usage(u32* pBlocksUsed = nullptr, u32* pBlocksFree = nullptr) { return mem_usage_impl(pBlocksUsed, pBlocksFree); }
	void				mem_compact();
	void				mem_counter_set(u32 _val) { stat_counter = _val; }
	u32					mem_counter_get() { return stat_counter; }

	void*				mem_alloc(size_t size);

	void*				mem_realloc(void* p, size_t size);
	void				mem_free(void* p);
};

extern XRCORE_API xrMemory Memory;

// delete
#include "xrMemory_subst_msvc.h"

// generic "C"-like allocations/deallocations
template <class T>
IC T*		xr_alloc		(size_t count)			{ return (T*)Memory.mem_alloc(count * sizeof(T)); }
template <class T>
IC void		xr_free			(T* &P) { if (P)		{ Memory.mem_free((void*)P); P = nullptr; }; }
IC void*	xr_malloc		(size_t size)			{ return Memory.mem_alloc(size); }
IC void*	xr_realloc		(void* P, size_t size)	{ return Memory.mem_realloc(P, size); }

XRCORE_API char* xr_strdup(const char* string);
#pragma warning( disable : 4595)  
IC void*	operator new		(size_t size)	{ return Memory.mem_alloc(size ? size : 1); }
IC void*	operator new[]		(size_t size)	{ return Memory.mem_alloc(size ? size : 1); }
IC void		operator delete		(void *p)		{ xr_free(p); }
IC void		operator delete[]	(void* p)		{ xr_free(p); }
#pragma warning( default : 4595)  

// POOL-ing
const		u32			mem_pools_count = 54;
const		u32			mem_pools_ebase = 16;
const		u32			mem_generic = mem_pools_count + 1;
extern		bool		mem_initialized;

XRCORE_API void vminfo(size_t *_free, size_t *reserved, size_t *committed);
XRCORE_API void log_vminfo();
