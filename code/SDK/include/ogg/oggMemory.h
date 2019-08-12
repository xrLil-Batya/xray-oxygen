#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
	typedef void* (*OggMemAllocFunc) (size_t);
	typedef void* (*OggMemCAllocFunc) (size_t, size_t);
	typedef void* (*OggMemReallocFunc) (void*, size_t);
	typedef void (*OggMemFreeFunc) (void*);

	extern OggMemAllocFunc		memOggAllocRoutine;
	extern OggMemCAllocFunc		memOggCAllocRoutine;
	extern OggMemReallocFunc	memOggReallocRoutine;
	extern OggMemFreeFunc		memOggFreeRoutine;

#ifdef __cplusplus
}
#endif