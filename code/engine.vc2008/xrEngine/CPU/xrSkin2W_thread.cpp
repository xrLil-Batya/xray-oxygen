#include "stdafx.h"
#include "xrCPU_Pipe.h"
#include "../xrCore/threadpool/ttapi.h"
#pragma hdrstop

extern xrSkin4W xrSkin4W_x86;

struct SKIN_PARAMS 
{
	LPVOID Dest;
	LPVOID Src;
	u32 Count;
	LPVOID Data;
};

void Skin4W_Stream(LPVOID lpvParams)
{
	SKIN_PARAMS* sp = (SKIN_PARAMS*)lpvParams;

	vertRender*		D = (vertRender*)sp->Dest;
	vertBoned4W*	S = (vertBoned4W*)sp->Src;
	u32				vCount = sp->Count;
	CBoneInstance*	Bones = (CBoneInstance*)sp->Data;

	xrSkin4W_x86(D, S, vCount, Bones);
}

void ENGINE_API xrSkin4W_thread(vertRender* D, vertBoned4W* S, u32 vCount, CBoneInstance* Bones)
{
	if (vCount < 16) 
	{
		xrSkin4W_x86(D, S, vCount, Bones);
		return;
	}

	u32 nWorkers = (u32)ttapi_GetWorkersCount();
	SKIN_PARAMS* sknParams = new SKIN_PARAMS[nWorkers];

	// Give ~1% more for the last worker
	// to minimize wait in final spin
	u32 nSlice = vCount / 32;

	u32 nStep = ((vCount - nSlice) / nWorkers);
	u32 nLast = vCount - nStep * (nWorkers - 1);

	for (u32 i = 0; i < nWorkers; ++i) 
	{
		sknParams[i].Dest = (void*)(D + i * nStep);
		sknParams[i].Src = (void*)(S + i * nStep);
		sknParams[i].Count = (i == (nWorkers - 1)) ? nLast : nStep;
		sknParams[i].Data = (void*)Bones;

		ttapi_AddTask(Skin4W_Stream, (void*)&sknParams[i]);
	}

	ttapi_RunAllWorkers();
	delete sknParams;
}

