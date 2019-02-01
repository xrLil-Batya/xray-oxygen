// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Engine.h"
#include "CPU\xrCPU_Pipe.h"
#include "tbb/task_scheduler_init.h"

CEngine Engine;
xrDispatchTable	PSGP;

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
	// Bind PSGP
	xrBind_PSGP(&PSGP, &CPU::Info);

	// Other stuff
	Engine.Sheduler.Initialize();
}

void CEngine::Destroy()
{
	Engine.Sheduler.Destroy();
	Engine.External.Destroy();

	//ttapi_Done();
	xr_delete(pTaskSheduler);
	std::memset(&PSGP, 0, sizeof(PSGP));
}
