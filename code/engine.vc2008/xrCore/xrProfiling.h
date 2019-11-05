// Giperion November 2019
// [EUREKA] 3.11.1

//////////////////////////////////////////////////////////////
// Desc		: Access to profiling backend
// Author	: Giperion
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////
#pragma once

class XRCORE_API xrProfiling
{
public:
	enum class eEngineFrame
	{
		Main,
		MAX
	};
public:
	void Initialize();

	// Frame
	void StartFrame(eEngineFrame Frame);
	void EndFrame(eEngineFrame Frame);

	// Memory allocation
	void StartAlloc(size_t size);
	void EndAlloc(void* mem, size_t size);

	void StartFree(void* mem);
	void EndFree(void* mem);

	void StartReAlloc(void* mem, size_t newSize);
	void EndReAlloc(void* oldMem, void** newMem, size_t newSize);

	// General control
	void PauseProfiling();
	void ResumeProfiling();

	// Thread naming
	void SetCurrentThreadName(const char* Name);

	// Tasks
	void StartTask(const char* name);
	void EndTask(const char* name);
private:
	bool bInitialized = false;
};

struct XRCORE_API xrProfilingTask
{
	xrProfilingTask(const char* name);
	~xrProfilingTask();

private:
	const char* name;
};

extern XRCORE_API xrProfiling Profiling;