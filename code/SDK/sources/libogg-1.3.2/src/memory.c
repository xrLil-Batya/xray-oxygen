//////////////////////////////////////////////////////////
// Desc   : Memory callbacks for Ogg and Vorbis
// Author : Giperion
//////////////////////////////////////////////////////////
// X-Ray Oxygen Engine (2016-2019)
//////////////////////////////////////////////////////////
// Giperion August 2019
// [EUREKA] 3.10.2

#include "stdint.h"
#include <ogg/oggMemory.h>
#include "ogg/os_types.h"
#include <stdlib.h>

void* oggDefaultMemMalloc(size_t size)
{
	return malloc(size);
}

void* oggDefaultMemCalloc(size_t num, size_t sizeOfElem)
{
	return calloc(num, sizeOfElem);
}

void* oggDefaultMemRealloc(void* ptr, size_t newSize)
{
	return realloc(ptr, newSize);
}

void oggDefaultMemFree(void* ptr)
{
	free(ptr);
}

OggMemAllocFunc		memOggAllocRoutine = oggDefaultMemMalloc;
OggMemCAllocFunc	memOggCAllocRoutine = oggDefaultMemCalloc;
OggMemReallocFunc	memOggReallocRoutine = oggDefaultMemRealloc;
OggMemFreeFunc		memOggFreeRoutine = oggDefaultMemFree;

void* _ogg_malloc(size_t size)
{
	return memOggAllocRoutine(size);
}

void* _ogg_calloc(size_t num, size_t SizeOfElem)
{
	return memOggCAllocRoutine(num, SizeOfElem);
}

void* _ogg_realloc(void* ptr, size_t newSize)
{
	return memOggReallocRoutine(ptr, newSize);
}

void _ogg_free(void* ptr)
{
	memOggFreeRoutine(ptr);
}

void SetAllocFunc(OggMemAllocFunc func)
{
	memOggAllocRoutine = func;
}