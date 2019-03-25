// xrPhysics.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "xrPhysics.h"

#include "../../3rd-party/ode/include/ode/memory.h"

#pragma comment(lib,"ode.lib")
#pragma comment(lib,"xrCDB.lib")
#pragma comment(lib,"xrEngine.lib")
#pragma comment(lib,"xrSound.lib")

#ifdef _MANAGED
#pragma managed(push, off)
#endif

int APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, void* lpReserved)
{
	lpReserved;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		auto ode_realloc = [](void *ptr, size_t oldsize, size_t newsize)
		{
			return xr_realloc(ptr, newsize); 
		};

		auto ode_free = [](void *ptr, size_t size) 
		{ 
			xr_free(ptr); 
		};

		dSetAllocHandler(xr_malloc);
		dSetReallocHandler(ode_realloc);
		dSetFreeHandler(ode_free);

		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

