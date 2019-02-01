#pragma once
#include <windows.h>

#if 0

#define TTAPI XRCORE_API
typedef void(*PTTAPI_WORKER_FUNC)(LPVOID lpWorkerParameters);
typedef PTTAPI_WORKER_FUNC LPPTTAPI_WORKER_FUNC;

extern "C"
{
	// Initializes subsystem
	// Returns zero for error, and number of workers on success
	size_t TTAPI ttapi_Init(processor_info* ID);

	// Destroys subsystem
	void TTAPI ttapi_Done();

	// Return number of workers
	size_t TTAPI ttapi_GetWorkersCount();

	// Adds new task
	// No more than TTAPI_HARDCODED_THREADS should be added
	void TTAPI ttapi_AddTask(LPPTTAPI_WORKER_FUNC lpWorkerFunc, LPVOID lpvWorkerFuncParams);

	// Runs and wait for all workers to complete job
	void TTAPI ttapi_RunAllWorkers();

	void TTAPI ttapi_example_taskentry(LPVOID param);
	void TTAPI ttapi_example2_taskentry(LPVOID param);
}

#endif