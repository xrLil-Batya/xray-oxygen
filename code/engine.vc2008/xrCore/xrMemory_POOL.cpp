#include "stdafx.h"
#pragma hdrstop

#include "xrMemory_POOL.h"
#include "xrMemory_align.h"

void	MEMPOOL::block_create	()
{
	// Allocate
	R_ASSERT				(0==list);
	list					= (u8*)		xr_aligned_offset_malloc	(s_sector,16,s_offset);

	// Partition
	for (u32 it=0; it<(s_count-1); it++)
	{
		u8*	E				= list + it*s_element;
		*access(E)			= E+s_element;
	}
	*access	(list+(s_count-1)*s_element)	= NULL;
	block_count				++;
}

void MEMPOOL::_initialize(u32 _element, u32 _sector, u32 _header)
{
	R_ASSERT		(_element < _sector/2);
	s_sector		= _sector;
	s_element		= _element;
	s_count			= s_sector/s_element;
	s_offset		= _header;
	list			= NULL;
	block_count		= 0;
}

void* MEMPOOL::create()
{
       std::lock_guard<decltype(cs)> lock(cs);
	if (0==list)	block_create();

	void* E			= list;
	list			= (u8*)*access(list);
	return			E;
}

void MEMPOOL::destroy(void* &P)
{
    std::lock_guard<decltype(cs)> lock(cs);
	*access(P)		= list;
	list			= (u8*)P;
}