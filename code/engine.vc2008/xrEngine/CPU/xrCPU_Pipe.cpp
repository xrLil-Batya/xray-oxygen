#include "stdafx.h"
#pragma hdrstop
#include "xrCPU_Pipe.h"
#include "../xrCore/threadpool/ttapi.h"

extern xrSkin1W			xrSkin1W_x86;
extern xrSkin2W			xrSkin2W_x86;
extern xrSkin3W			xrSkin3W_x86;
extern xrSkin4W			xrSkin4W_x86;

extern xrSkin1W			xrSkin1W_SSE;
extern xrSkin2W			xrSkin2W_SSE;
extern xrSkin3W			xrSkin3W_SSE;
extern xrSkin4W			xrSkin4W_SSE;
extern xrSkin2W			xrSkin2W_3DNow;
extern xrSkin1W			xrSkin1W_3DNow;

extern xrSkin4W			xrSkin4W_thread;

xrSkin4W* skin4W_func = nullptr;

extern xrPLC_calc3		PLC_calc3_x86;
extern xrPLC_calc3		PLC_calc3_SSE;


void xrBind_PSGP(xrDispatchTable* T, processor_info* ID)
{
	// generic
	T->skin1W = xrSkin1W_x86;
	T->skin2W = xrSkin2W_x86;
	T->skin3W = xrSkin3W_x86;
	T->skin4W = xrSkin4W_x86;
	skin4W_func = xrSkin4W_x86;
	T->PLC_calc3 = PLC_calc3_x86;

	// SSE
	if (ID->hasFeature(CPUFeature::SSE))
	{
		T->PLC_calc3 = PLC_calc3_SSE;
#ifndef _M_X64
		T->skin1W = xrSkin1W_SSE;
		T->skin2W = xrSkin2W_SSE;
		T->skin3W = xrSkin3W_SSE;
		T->skin4W = xrSkin4W_SSE;
		skin4W_func = xrSkin4W_SSE;
	}
	else if (ID->hasFeature(CPUFeature::AMD_3DNow))
	{
		T->skin1W = xrSkin1W_3DNow;
		T->skin2W = xrSkin2W_3DNow;
#endif
	}
	// Init helper threads
	ttapi_Init(ID);

#ifdef TEST_TTAPI
	for (int iteration = 0; iteration < 1000; ++iteration)
	{
		volatile DWORD TestCounter = 0;
		for (int taskID = 0; taskID < 10000; ++taskID)
		{
			ttapi_AddTask(ttapi_example2_taskentry, (LPVOID)&TestCounter);
		}
		ttapi_RunAllWorkers();

		if (TestCounter != 10000)
		{
			Msg("! ttapi test failed");
		}
	}
#endif

	// We can use threading
	if (ttapi_GetWorkersCount() > 1)
		T->skin4W = xrSkin4W_thread;
}
