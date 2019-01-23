#include "stdafx.h"
#pragma hdrstop
#include "xrCPU_Pipe.h"
#include "tbb/task_scheduler_init.h"

extern xrSkin1W			xrSkin1W_x86;
extern xrSkin2W			xrSkin2W_x86;
extern xrSkin3W			xrSkin3W_x86;
extern xrSkin4W			xrSkin4W_x86;
extern xrSkin4W			xrSkin4W_thread;

extern xrPLC_calc3		PLC_calc3_x86;
extern xrPLC_calc3		PLC_calc3_SSE;

tbb::task_scheduler_init* pTaskSheduler = nullptr;

void xrBind_PSGP(xrDispatchTable* T, processor_info* ID)
{
	// generic
	T->skin1W = xrSkin1W_x86;
	T->skin2W = xrSkin2W_x86;
	T->skin3W = xrSkin3W_x86;
	T->skin4W = xrSkin4W_thread;

	// SSE
	if (ID->hasFeature(CPUFeature::SSE))
	{
		T->PLC_calc3 = PLC_calc3_SSE;
	}
	else
	{
		T->PLC_calc3 = PLC_calc3_x86;
	}
	// Init helper threads
	//ttapi_Init(ID);
	pTaskSheduler = new tbb::task_scheduler_init(CPU::Info.n_threads);
}
