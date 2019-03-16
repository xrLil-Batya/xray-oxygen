#include "stdafx.h"
#include "xrCPU_Pipe.h"
#include "tbb/task.h"
#include "tbb/task_group.h"

extern xrSkin4W xrSkin4W_x86;

struct SKIN_PARAMS 
{
	LPVOID Dest;
	LPVOID Src;
	u32 Count;
	LPVOID Data;
};

void Skin4W_Stream(SKIN_PARAMS* sp)
{
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

	tbb::task_group SkinningTasks;

	constexpr size_t SkinParamSize = 64;
	SKIN_PARAMS sknParams[SkinParamSize];
	u32 WorkerCounts = CPU::Info.n_threads;
	R_ASSERT(SkinParamSize > WorkerCounts);

	// Give ~1% more for the last worker
	// to minimize wait in final spin
	u32 nSlice = vCount / 32;

	u32 nStep = ((vCount - nSlice) / WorkerCounts);
	u32 nLast = vCount - nStep * (WorkerCounts - 1);
	for (u32 i = 0; i < WorkerCounts; ++i)
	{
		sknParams[i].Dest = (void*)(D + i * nStep);
		sknParams[i].Src = (void*)(S + i * nStep);
		sknParams[i].Count = (i == (WorkerCounts - 1)) ? nLast : nStep;
		sknParams[i].Data = (void*)Bones;

		SkinningTasks.run([&sknParams, i]()
		{
			Skin4W_Stream(&sknParams[i]);
		});
	}

	SkinningTasks.wait();
}

