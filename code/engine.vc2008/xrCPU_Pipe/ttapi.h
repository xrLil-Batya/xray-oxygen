#pragma once
#include <windows.h>

typedef void(*PTTAPI_WORKER_FUNC)(LPVOID lpWorkerParameters);
typedef PTTAPI_WORKER_FUNC LPPTTAPI_WORKER_FUNC;

#ifdef XRCPU_PIPE_EXPORTS
#define TTAPI __declspec(dllexport)
#else // XRCPU_PIPE_EXPORTS
#define TTAPI __declspec(dllimport)
#endif // XRCPU_PIPE_EXPORTS

extern "C" {

	// Initializes subsystem
	// Returns zero for error, and number of workers on success
	size_t TTAPI ttapi_Init(processor_info* ID);

	// Destroys subsystem
	void TTAPI ttapi_Done();

	// Return number of workers
	size_t TTAPI ttapi_GetWorkersCount();

	// Adds new task
	// No more than TTAPI_HARDCODED_THREADS should be added
	void TTAPI ttapi_AddWorker(LPPTTAPI_WORKER_FUNC lpWorkerFunc, LPVOID lpvWorkerFuncParams);

	// Runs and wait for all workers to complete job
	void TTAPI ttapi_RunAllWorkers();

}