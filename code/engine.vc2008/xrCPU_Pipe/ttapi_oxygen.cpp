#include "stdafx.h"
#include "ttapi_oxygen.h"

PTP_POOL hPool = nullptr;

void NTAPI ttapi_worker_threadentry(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);

DWORD WorkerCount = 0;

struct ttapi_ThreadParams
{
	LPPTTAPI_WORKER_FUNC Func;
	LPVOID				 FuncParams;
	HANDLE				 CompletionEvent;

	ttapi_ThreadParams(LPPTTAPI_WORKER_FUNC InFunc, LPVOID InFuncParams, HANDLE InCompletionEvent)
		: Func(InFunc), FuncParams(InFuncParams), CompletionEvent(InCompletionEvent)
	{}
};

xr_vector<HANDLE> WorkerEvents;
CRITICAL_SECTION WorkerEventsGuard;


size_t TTAPI ttapi_Init(processor_info* ID)
{
	R_ASSERT(hPool == nullptr);
	hPool = CreateThreadpool(NULL);
	R_ASSERT(hPool != nullptr);

	SetThreadpoolThreadMinimum(hPool, ID->n_threads / 2);
	SetThreadpoolThreadMaximum(hPool, ID->n_threads);
	WorkerCount = ID->n_threads;

	InitializeCriticalSection(&WorkerEventsGuard);

	return ID->n_threads;
}

void TTAPI ttapi_Done()
{
	R_ASSERT(hPool != nullptr);
	ttapi_RunAllWorkers();
	CloseThreadpool(hPool);
	hPool = nullptr;
	WorkerCount = 0;
	DeleteCriticalSection(&WorkerEventsGuard);
}

size_t TTAPI ttapi_GetWorkersCount()
{
	return WorkerCount;
}

void TTAPI ttapi_AddTask(LPPTTAPI_WORKER_FUNC lpWorkerFunc, LPVOID lpvWorkerFuncParams)
{
	HANDLE hWorkerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ttapi_ThreadParams* pParams = new ttapi_ThreadParams(lpWorkerFunc, lpvWorkerFuncParams, hWorkerEvent);
	PTP_WORK hWork = CreateThreadpoolWork(ttapi_worker_threadentry, pParams, NULL);
	VERIFY(hWork != nullptr);

	EnterCriticalSection(&WorkerEventsGuard);
		WorkerEvents.push_back(hWorkerEvent);
	LeaveCriticalSection(&WorkerEventsGuard);

	SubmitThreadpoolWork(hWork);
}

void TTAPI ttapi_RunAllWorkers()
{
	xr_vector<HANDLE> WorkerEventsCopy;
	EnterCriticalSection(&WorkerEventsGuard);
		WorkerEventsCopy.reserve(WorkerEvents.size());
		for(HANDLE hEvent : WorkerEvents)
		{
			//Duplicate handle, because original handle closing in worker thread
			HANDLE hDuplicatedHandle = NULL;
			DuplicateHandle(GetCurrentProcess(), hEvent, GetCurrentProcess(), &hDuplicatedHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
			WorkerEventsCopy.push_back(hDuplicatedHandle);
		}
	LeaveCriticalSection(&WorkerEventsGuard);
	
	DWORD EventsCount = WorkerEventsCopy.size();
	HANDLE* pHandles = WorkerEventsCopy.data();
	DWORD WaitIterations = EventsCount / MAXIMUM_WAIT_OBJECTS;
	if ((EventsCount % MAXIMUM_WAIT_OBJECTS) > 0) ++WaitIterations;
	for (int WaitIter = 0; WaitIter < WaitIterations; ++WaitIter)
	{
		DWORD Offset = MAXIMUM_WAIT_OBJECTS * WaitIter;
		DWORD CurrentEventStack = WorkerEventsCopy.size() - Offset;
		CurrentEventStack = CurrentEventStack > MAXIMUM_WAIT_OBJECTS ? MAXIMUM_WAIT_OBJECTS : CurrentEventStack;
		WaitForMultipleObjects(CurrentEventStack, pHandles + Offset, TRUE, INFINITE);
	}

	for (HANDLE hEvent : WorkerEventsCopy)
	{
		CloseHandle(hEvent);
	}
}

void NTAPI ttapi_worker_threadentry(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work)
{
	ttapi_ThreadParams* pParams = (ttapi_ThreadParams*)Context;
	pParams->Func(pParams->FuncParams);
	SetEvent(pParams->CompletionEvent);

	EnterCriticalSection(&WorkerEventsGuard);
		CloseHandle(pParams->CompletionEvent);
		for (auto iter = WorkerEvents.begin(); iter != WorkerEvents.end(); ++iter)
		{
			if (*iter == pParams->CompletionEvent)
			{
				iter = WorkerEvents.erase(iter);
				break;
			}
		}
	LeaveCriticalSection(&WorkerEventsGuard);

	delete pParams;
}
