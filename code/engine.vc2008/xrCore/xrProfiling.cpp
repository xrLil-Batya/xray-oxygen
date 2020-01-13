// Giperion November 2019
// [EUREKA] 3.11.1

//////////////////////////////////////////////////////////////
// Desc		: Access to profiling backend
// Author	: Giperion
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
// Allowed for OpenXRay
//////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "xrProfiling.h"
#include "IntelVTune/ittnotify.h"

XRCORE_API xrProfiling Profiling;

#define APP_DOMAIN "Oxygen Engine 2.0"

struct TrackedID
{
	__itt_id ID;
	bool bStarted;
};

struct Task
{
	__itt_id Id;
	__itt_string_handle* name;
};

struct ProfilingPrivate
{
	__itt_domain* Domain;
	TrackedID IDs[(int)xrProfiling::eEngineFrame::MAX];
	__itt_heap_function HeapFunctions[3];

	std::map<const char*, Task> Tasks;
} Private;

void xrProfiling::Initialize(eProfilingMode InMode)
{
	mode = InMode;
	R_ASSERT2(!bInitialized, "xrProfiling initialize should be called once");

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		frontBacket = &profilingBacket1;
		backBacket = &profilingBacket2;
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
	{
		Private.Domain = __itt_domain_create(APP_DOMAIN);
		__itt_thread_set_name("X-Ray Main Thread");

		for (int i = 0; i < (int)eEngineFrame::MAX; i++)
		{
			TrackedID& ID = Private.IDs[i];
			ID.ID = __itt_null;
			ID.ID.d1 = i;
			ID.bStarted = false;
			__itt_id_create(Private.Domain, ID.ID);
		}

		Private.HeapFunctions[0] = __itt_heap_function_create("mimalloc_malloc", APP_DOMAIN);
		Private.HeapFunctions[1] = __itt_heap_function_create("mimalloc_free", APP_DOMAIN);
		Private.HeapFunctions[2] = __itt_heap_function_create("mimalloc_realloc", APP_DOMAIN);
	}
		break;
	default:
		break;
	}

	bInitialized = true;
}

void xrProfiling::StartFrame(eEngineFrame Frame)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
	{
		TrackedID& FrameID = Private.IDs[(int)Frame];
		R_ASSERT2(!FrameID.bStarted, "Profiling frame started again!");
		FrameID.bStarted = true;
		__itt_frame_begin_v3(Private.Domain, &FrameID.ID);
	}
		break;
	default:
		break;
	}
}

void xrProfiling::EndFrame(eEngineFrame Frame)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
	{
		// swap backets
		if (frontBacket == &profilingBacket1)
		{
			frontBacket = &profilingBacket2;
			backBacket  = &profilingBacket1;
		}
		else
		{
			frontBacket = &profilingBacket1;
			backBacket  = &profilingBacket2;
		}
	}
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
	{
		TrackedID& FrameID = Private.IDs[(int)Frame];
		R_ASSERT2(FrameID.bStarted, "Profiling frame is not started, but we try to end!");
		FrameID.bStarted = false;
		__itt_frame_end_v3(Private.Domain, &FrameID.ID);
	}
		break;
	default:
		break;
	}
}

void xrProfiling::StartAlloc(size_t size)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
		__itt_heap_allocate_begin(Private.HeapFunctions[0], size, 0);
		break;
	default:
		break;
	}
}

void xrProfiling::EndAlloc(void* mem, size_t size)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
		__itt_heap_allocate_end(Private.HeapFunctions[0], &mem, size, 0);
		break;
	default:
		break;
	}
}

void xrProfiling::StartFree(void* mem)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
		__itt_heap_free_begin(Private.HeapFunctions[1], mem);
		break;
	default:
		break;
	}
}

void xrProfiling::EndFree(void* mem)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
		__itt_heap_free_end(Private.HeapFunctions[1], mem);
		break;
	default:
		break;
	}
}

void xrProfiling::StartReAlloc(void* mem, size_t newSize)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
		__itt_heap_reallocate_begin(Private.HeapFunctions[2], mem, newSize, 0);
		break;
	default:
		break;
	}
}

void xrProfiling::EndReAlloc(void* oldMem, void** newMem, size_t newSize)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
		__itt_heap_reallocate_end(Private.HeapFunctions[2], oldMem, newMem, newSize, 0);
		break;
	default:
		break;
	}
}

void xrProfiling::PauseProfiling()
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		bEngineProfilingActive = false;
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
		__itt_pause();
		break;
	default:
		break;
	}
}

void xrProfiling::ResumeProfiling()
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
		bEngineProfilingActive = true;
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
		__itt_resume();
		break;
	default:
		break;
	}
}

void xrProfiling::SetCurrentThreadName(const char* Name)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case eProfilingMode::Engine:
		break;
	case eProfilingMode::IntelProfiler:
		__itt_thread_set_name(Name);
		break;
	default:
		break;
	}
}

void xrProfiling::StartTask(const char* name)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
	{
		if (bEngineProfilingActive)
		{
			profilingBacketGuard.Enter();
			(*backBacket)[name].Start = CPU::QPC();
			profilingBacketGuard.Leave();
		}
	}
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
	{
		__itt_id taskId;
		__itt_string_handle* strHandle = nullptr;
		auto TaskIter = Private.Tasks.find(name);
		if (TaskIter == Private.Tasks.end())
		{
			taskId = __itt_id_make((void*)name, 0);
			__itt_id_create(Private.Domain, taskId);
			strHandle = __itt_string_handle_create(name);
			Private.Tasks.emplace(std::make_pair(name, Task{ taskId, strHandle }));
		}
		else
		{
			taskId = TaskIter->second.Id;
			strHandle = TaskIter->second.name;
		}

		__itt_task_begin(Private.Domain, taskId, __itt_null, strHandle);
	}
		break;
	default:
		break;
	}


}

void xrProfiling::EndTask(const char* name)
{
	if (!bInitialized) return;

	switch (mode)
	{
	case xrProfiling::eProfilingMode::Engine:
	{
		if (bEngineProfilingActive)
		{
			profilingBacketGuard.Enter();
			(*backBacket)[name].End = CPU::QPC();
			profilingBacketGuard.Leave();
		}
	}
		break;
	case xrProfiling::eProfilingMode::IntelProfiler:
	{
		__itt_task_end(Private.Domain);
	}
		break;
	default:
		break;
	}

}

xrProfilingTask::xrProfilingTask(const char* InName)
	: name(InName)
{
	Profiling.StartTask(InName);
}

xrProfilingTask::~xrProfilingTask()
{
	Profiling.EndTask(name);
}
