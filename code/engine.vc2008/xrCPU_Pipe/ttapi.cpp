#include "stdafx.h"

#if 0
typedef struct TTAPI_WORKER_PARAMS {
	volatile long			vlFlag;
	LPPTTAPI_WORKER_FUNC	lpWorkerFunc;
	LPVOID					lpvWorkerFuncParams;
	DWORD					dwPadding[13];
} *PTTAPI_WORKER_PARAMS;

typedef PTTAPI_WORKER_PARAMS LPTTAPI_WORKER_PARAMS;

static LPHANDLE ttapi_threads_handles = nullptr;
static bool ttapi_initialized = false;
static size_t ttapi_workers_count = 0;
static size_t ttapi_threads_count = 0;
static size_t ttapi_assigned_workers = 0;
static LPTTAPI_WORKER_PARAMS ttapi_worker_params = nullptr;

static size_t ttapi_dwFastIter = 0;
static size_t ttapi_dwSlowIter = 0;

struct {
	volatile long size;
	DWORD dwPadding[15];
} ttapi_queue_size;

DWORD WINAPI ttapiThreadProc(LPVOID lpParameter)
{
	LPTTAPI_WORKER_PARAMS pParams = (LPTTAPI_WORKER_PARAMS)lpParameter;
	size_t i, dwFastIter = ttapi_dwFastIter, dwSlowIter = ttapi_dwSlowIter;

	while (true) {
		// Wait. Fast
		for (i = 0; i < dwFastIter; ++i) 
		{
			if (!pParams->vlFlag)
				goto process;
			_mm_pause();
		}

		// Moderate
		for (i = 0; i < dwSlowIter; ++i) 
		{
			if (!pParams->vlFlag)
				goto process;

			SwitchToThread();
		}

		// Slow
		while (pParams->vlFlag)
			Sleep(100);

	process:
		pParams->vlFlag = 1;

		if (pParams->lpWorkerFunc)
			pParams->lpWorkerFunc(pParams->lpvWorkerFuncParams);
		else
			break;

		_InterlockedDecrement(&ttapi_queue_size.size);

	} // while

	return 0;
}

typedef struct tagTHREADNAME_INFO {
	DWORD dwType;
	LPCSTR szName;
	DWORD dwThreadID;
	DWORD dwFlags;
} THREADNAME_INFO;

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
	THREADNAME_INFO info;
	{
		info.dwType = 0x1000;
		info.szName = szThreadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;
	}
	__try
	{
#ifdef _M_X64
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
#else
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (DWORD*)&info);
#endif
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

size_t ttapi_Init(processor_info* ID)
{
	if (ttapi_initialized)
		return ttapi_workers_count;

	// System Info
	ttapi_workers_count = ID->n_cores;
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	DWORD i, dwNumIter;
	volatile DWORD dwDummy = 1;
	LARGE_INTEGER liFrequency, liStart, liEnd;

	QueryPerformanceFrequency(&liFrequency);

	// Get fast spin-loop timings
	dwNumIter = 100000000;

	QueryPerformanceCounter(&liStart);
	for (i = 0; i < dwNumIter; ++i) {
		if (dwDummy == 0)
			goto process1;
		_mm_pause();
	}
process1:
	QueryPerformanceCounter(&liEnd);

	// We want 1/25 (40ms) fast spin-loop
	ttapi_dwFastIter = (dwNumIter * liFrequency.QuadPart) / ((liEnd.QuadPart - liStart.QuadPart) * 25);
#ifdef DEBUG
	Msg( "fast spin-loop iterations : %u" , ttapi_dwFastIter );
#endif
	// Get slow spin-loop timings
	dwNumIter = 10000000;

	QueryPerformanceCounter(&liStart);
	for (i = 0; i < dwNumIter; ++i) {
		if (dwDummy == 0)
			goto process2;
		SwitchToThread();
	}
process2:
	QueryPerformanceCounter(&liEnd);

	// We want 1/2 (500ms) slow spin-loop
	ttapi_dwSlowIter = (dwNumIter * liFrequency.QuadPart) / ((liEnd.QuadPart - liStart.QuadPart) * 2);
#ifdef DEBUG
	Msg("fast spin-loop iterations : %u", ttapi_dwSlowIter);
#endif
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

	// Check for override from command line
	char szSearchFor[] = "-max-threads";
	char* pszTemp = strstr(GetCommandLine(), szSearchFor);
	size_t dwOverride = 0;
#ifdef _M_X64
	if (pszTemp && sscanf_s(pszTemp + strlen(szSearchFor), "%zu", &dwOverride) &&
#else
	if (pszTemp && sscanf_s(pszTemp + strlen(szSearchFor), "%u", &dwOverride) && 
#endif
		(dwOverride >= 1) && (dwOverride < ttapi_workers_count))
				ttapi_workers_count = dwOverride;

	// Number of helper threads
	ttapi_threads_count = ttapi_workers_count - 1;
	ttapi_threads_handles = (LPHANDLE)malloc(sizeof(HANDLE)*ttapi_threads_count);
	ttapi_worker_params = (PTTAPI_WORKER_PARAMS)malloc(sizeof(TTAPI_WORKER_PARAMS)*ttapi_workers_count);
	// Creating control structures
	if (!(ttapi_threads_handles || ttapi_worker_params))
		return 0;

	// Clearing params
	for (auto j = 0; j < ttapi_workers_count; j++)
		memset(&ttapi_worker_params[j], 0, sizeof(TTAPI_WORKER_PARAMS));

	char szThreadName[64];
	DWORD dwThreadId = 0,
		  dwAffinitiMask = ID->affinity_mask;
	auto dwCurrentMask = 0x01;

	// Setting affinity
	while (!(dwAffinitiMask & dwCurrentMask))
		dwCurrentMask <<= 1;

	//SetThreadAffinityMask(GetCurrentThread(), dwCurrentMask);

	// Creating threads
	for (u32 it = 0; it < ttapi_threads_count; it++)
	{
		// Initializing "enter" "critical section"
		ttapi_worker_params[it].vlFlag = 1;
		ttapi_threads_handles[it] = CreateThread(nullptr, 0, &ttapiThreadProc, &ttapi_worker_params[it], 0, &dwThreadId);
		if (!ttapi_threads_handles[it])
			return 0;

		// Setting affinity
		do dwCurrentMask <<= 1;
		while (!(dwAffinitiMask & dwCurrentMask));

		//SetThreadAffinityMask(ttapi_threads_handles[it], dwCurrentMask);

		// Setting thread name
		sprintf_s(szThreadName, "Helper Thread #%u", it);
		SetThreadName(dwThreadId, szThreadName);
	}
	ttapi_initialized = true;

	return ttapi_workers_count;
}

size_t ttapi_GetWorkersCount()
{
	return ttapi_workers_count;
}

// We do not check for overflow here to be faster
// Assume that caller is smart enough to use ttapi_GetWorkersCount() to get number of available slots
void ttapi_AddTask(LPPTTAPI_WORKER_FUNC lpWorkerFunc, LPVOID lpvWorkerFuncParams)
{
	// Assigning parameters
	ttapi_worker_params[ttapi_assigned_workers].lpWorkerFunc = lpWorkerFunc;
	ttapi_worker_params[ttapi_assigned_workers].lpvWorkerFuncParams = lpvWorkerFuncParams;

	ttapi_assigned_workers++;
}

void ttapi_RunAllWorkers()
{
	size_t ttapi_thread_workers = (ttapi_assigned_workers - 1);

	if (ttapi_thread_workers) 
	{
		// Setting queue size
		ttapi_queue_size.size = (long)ttapi_thread_workers;
		// Starting all workers except the last
		for (auto i = 0; i < ttapi_thread_workers; ++i)
			_InterlockedExchange(&ttapi_worker_params[i].vlFlag, 0);
	}
		// Running 'the only/last' worker in current thread
		ttapi_worker_params[ttapi_thread_workers].lpWorkerFunc(ttapi_worker_params[ttapi_thread_workers].lpvWorkerFuncParams);

		// Waiting task queue to become empty
		if (ttapi_thread_workers)
			while (ttapi_queue_size.size)
				_mm_pause();
	
	// Cleaning active workers count
	ttapi_assigned_workers = 0;
}

void ttapi_Done()
{
	if (!ttapi_initialized)
		return;

	// Asking helper threads to terminate
	for (auto i = 0; i < ttapi_threads_count; i++) 
	{
		ttapi_worker_params[i].lpWorkerFunc = nullptr;
		_InterlockedExchange(&ttapi_worker_params[i].vlFlag, 0);
	}

	// Waiting threads for completion
	
	WaitForMultipleObjects((DWORD)ttapi_threads_count, ttapi_threads_handles, true, INFINITE);
	
	// Freeing resources
	free(ttapi_threads_handles);		
	ttapi_threads_handles = nullptr;
	free(ttapi_worker_params);		
	ttapi_worker_params = nullptr;

	ttapi_workers_count = 0;
	ttapi_threads_count = 0;
	ttapi_assigned_workers = 0;

	ttapi_initialized = false;
}

#endif