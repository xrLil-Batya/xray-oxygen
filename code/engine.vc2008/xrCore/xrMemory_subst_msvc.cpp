#include "stdafx.h"
#pragma hdrstop
#include "xrMemory.h"

void* xrMemory::mem_alloc(size_t size)
{
	stat_calls++;
	return malloc(size);
}

void xrMemory::mem_free(void* P)
{
	stat_calls++;
	free(P);
}

void* xrMemory::mem_realloc(void* P, size_t size)
{
	stat_calls++;
	return realloc(P, size);
}
