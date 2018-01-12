#include "stdafx.h"
#pragma hdrstop
#include "xrMemory_align.h"

MEMPOOL mem_pools[mem_pools_count];

IC void* xrMemHead(void* size) { return (void*)(((u8*)size) + 1); }

// MSVC
ICF	u8* acc_header(void* P) { u8* _P = (u8*)P; return	_P - 1; }
ICF	u32 get_header(void* P) { return (u32)*acc_header(P); }

ICF	u32	get_pool(size_t size)
{
	u32 pid = u32(size / mem_pools_ebase);
	if (pid >= mem_pools_count)	return mem_generic;
	else return pid;
}


void* xrMemory::mem_alloc(size_t size)
{
	stat_calls++;

	ConditionalInitPureAlloc();


	if (use_pure_alloc)
	{
		return malloc(size);
	}

	u32 _footer = 4;
	void* _ptr = 0;
	void* _real = xr_aligned_offset_malloc(1 + size + _footer, 16, 0x1);

	if (!mem_initialized)
	{
		// generic
		_ptr = xrMemHead(_real);
		*acc_header(_ptr) = mem_generic;
	}
	else 
	{
		//	accelerated
		u32	pool = get_pool(1 + size + _footer);
		if (mem_generic == pool)
		{
			// generic
			_ptr = xrMemHead(_real);
			*acc_header(_ptr) = mem_generic;
		}
		else {
			// pooled
			//	Already reserved when getting pool id
			_real = mem_pools[pool].create();
			_ptr = xrMemHead(_real);
			*acc_header(_ptr) = (u8)pool;
		}
	}
	return	_ptr;
}

void xrMemory::mem_free(void* P)
{
	stat_calls++;

	if (use_pure_alloc)
	{
		free(P);
		return;
	}

	u32	pool = get_header(P);
	void* _real = (void*)(((u8*)P) - 1);
	if (mem_generic == pool)
	{
		// generic
		xr_aligned_free(_real);
	}
	else
	{
		// pooled
		VERIFY2(pool<mem_pools_count, "Memory corruption");
		mem_pools[pool].destroy(_real);
	}
}

void* xrMemory::mem_realloc(void* P, size_t size)
{
	stat_calls++;
	if (use_pure_alloc)
	{
		return realloc(P, size);
	}
	if (!P) 
	{
		return mem_alloc(size);
	}

	u32 p_current = get_header(P);
	//	FX: Reserve 1 byte for xrMemory header (4 + size + 1)
	u32 p_new = get_pool(size + 5);
	u32 p_mode = 1;

	if (mem_generic == p_current) 
	{
		p_mode = (p_new < p_current) ? 2 : 0;
	}

	void* _real = (void*)(((u8*)P) - 1);
	void* _ptr = nullptr;
	switch (p_mode)
	{
	//	Igor: Reserve 1 byte for xrMemory header
		case 0:
		{
			const u32 _footer = 4;
			void* _real2 = xr_aligned_offset_realloc(_real, 1 + size + _footer, 16, 0x1);
			_ptr = xrMemHead(_real2);
			*acc_header(_ptr) = mem_generic;
		}
		case 1:
		{
			// pooled realloc
			R_ASSERT2(p_current<mem_pools_count, "Memory corruption");
			u32 s_current = mem_pools[p_current].get_element();
			u32 s_dest = (u32)size;
			void* p_old = P;

			void* is_p_new = mem_alloc(size);
			//	Don't bother in this case?
			std::memcpy(is_p_new, p_old, std::min(s_current - 1, s_dest));
			mem_free(p_old);
			_ptr = is_p_new;
		}
		case 2:
		{
			// relocate into another mmgr(pooled) from real
			void* p_old = P;
			void* _p_new = mem_alloc(size);
			std::memcpy(_p_new, p_old, size);
			mem_free(p_old);
			_ptr = _p_new;
		}
	}

	return	_ptr;
}
