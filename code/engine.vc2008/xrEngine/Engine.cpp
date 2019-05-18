// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Engine.h"
#include "tbb/task_scheduler_init.h"

CEngine Engine;
tbb::task_scheduler_init* pTaskSheduler = nullptr;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEngine::CEngine()
{
}

CEngine::~CEngine()
{
}

ENGINE_API void CEngine::Initialize(void)
{
	// TBB Init
	pTaskSheduler = new tbb::task_scheduler_init(CPU::Info.n_threads);

	// Other stuff
	Engine.Sheduler.Initialize();
}

void CEngine::Destroy()
{
	Engine.Sheduler.Destroy();
	Engine.External.Destroy();

	xr_delete(pTaskSheduler);
}
