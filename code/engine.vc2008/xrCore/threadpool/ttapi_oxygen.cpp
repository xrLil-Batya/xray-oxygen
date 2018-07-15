#include "stdafx.h"
#include "../../FrayBuildConfig.hpp"
#include "ttapi.h"


PTP_POOL hPool = nullptr;
PTP_CLEANUP_GROUP hCleanupEnv = nullptr;
TP_CALLBACK_ENVIRON CallbackEnv;

void NTAPI ttapi_worker_threadentry(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);

DWORD WorkerCount = 0;

struct ttapi_ThreadParams
{
	LPPTTAPI_WORKER_FUNC Func;
	LPVOID FuncParams;

	ttapi_ThreadParams(LPPTTAPI_WORKER_FUNC InFunc, LPVOID InFuncParams) : Func(InFunc), FuncParams(InFuncParams)
	{}
};

size_t TTAPI ttapi_Init(processor_info* ID)
{
	R_ASSERT(hPool == nullptr);
	R_ASSERT(hCleanupEnv == nullptr);

	hPool = CreateThreadpool(NULL);
	R_ASSERT(hPool != nullptr);

	InitializeThreadpoolEnvironment(&CallbackEnv);
	SetThreadpoolCallbackPool(&CallbackEnv, hPool);

	hCleanupEnv = CreateThreadpoolCleanupGroup();
	R_ASSERT(hCleanupEnv != nullptr);
	SetThreadpoolCallbackCleanupGroup(&CallbackEnv, hCleanupEnv, NULL);

	WorkerCount = ID->n_threads * 2;

	SetThreadpoolThreadMinimum(hPool, ID->n_threads);
	SetThreadpoolThreadMaximum(hPool, WorkerCount);

	return WorkerCount;
}

void TTAPI ttapi_Done()
{
	R_ASSERT(hPool != nullptr);
	R_ASSERT(hCleanupEnv != nullptr);
	ttapi_RunAllWorkers();

	CloseThreadpoolCleanupGroup(hCleanupEnv);
	DestroyThreadpoolEnvironment(&CallbackEnv);
	CloseThreadpool(hPool);
	hPool = nullptr;
	WorkerCount = 0;
}

size_t TTAPI ttapi_GetWorkersCount()
{
	return WorkerCount;
}

void TTAPI ttapi_AddTask(LPPTTAPI_WORKER_FUNC lpWorkerFunc, LPVOID lpvWorkerFuncParams)
{
	ttapi_ThreadParams* pParams = new ttapi_ThreadParams(lpWorkerFunc, lpvWorkerFuncParams);
	PTP_WORK hWork = CreateThreadpoolWork(ttapi_worker_threadentry, pParams, &CallbackEnv);
	VERIFY(hWork != nullptr);

	SubmitThreadpoolWork(hWork);
}

void TTAPI ttapi_RunAllWorkers()
{
	CloseThreadpoolCleanupGroupMembers(hCleanupEnv, FALSE, NULL);
}

void NTAPI ttapi_worker_threadentry(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work)
{
	ttapi_ThreadParams* pParams = (ttapi_ThreadParams*)Context;
	pParams->Func(pParams->FuncParams);
	delete pParams;
}

void TTAPI ttapi_example_taskentry(LPVOID param)
{
	volatile int Dummy = 2;
	Dummy += 42;
	Dummy /= 2;
}

void TTAPI ttapi_example2_taskentry(LPVOID param)
{
	volatile DWORD* pCounter = (DWORD*)param;
	InterlockedIncrement(pCounter);
}

