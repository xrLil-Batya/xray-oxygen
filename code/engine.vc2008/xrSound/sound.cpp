#include "stdafx.h"
#include <ogg/oggMemory.h>

#include "SoundRender_CoreA.h"

XRSOUND_API xr_token* snd_devices_token = nullptr;
XRSOUND_API u32 snd_device_id = u32(-1);

void* xrOggMemMalloc(size_t size)
{
	return Memory.mem_alloc(size);
}

void* xrOggMemCalloc(size_t num, size_t sizeOfElem)
{
	void* ptr = Memory.mem_alloc(num * sizeOfElem);
	ZeroMemory(ptr, num * sizeOfElem);
	return ptr;
}

void* xrOggMemRealloc(void* ptr, size_t newSize)
{
	return Memory.mem_realloc(ptr, newSize);
}

void xrOggMemFree(void* ptr)
{
	return Memory.mem_free(ptr);
}


void CSound_manager_interface::_create(int stage)
{
	if (!stage)
	{
		SoundRender = new CSoundRender_CoreA();
		Sound = SoundRender;

		SoundRender->bPresent = !(strstr(Core.Params, "-nosound"));
	}

	if (SoundRender->bPresent)
	{
		memOggAllocRoutine = xrOggMemMalloc;
		memOggCAllocRoutine = xrOggMemCalloc;
		memOggReallocRoutine = xrOggMemRealloc;
		memOggFreeRoutine = xrOggMemFree;
		Sound->_initialize(stage);
	}
}

void CSound_manager_interface::_destroy()
{
	Sound->_clear();
	xr_delete(SoundRender);
	Sound = nullptr;
}