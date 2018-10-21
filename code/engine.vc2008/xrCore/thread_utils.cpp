#include "stdafx.h"
#include "thread_utils.h"

xrCriticalSection::xrCriticalSection() : isLocked(false)
{
	InitializeCriticalSectionAndSpinCount(&Section, 250);
}

xrCriticalSection::~xrCriticalSection()
{
	DeleteCriticalSection(&Section);
}

void xrCriticalSection::Enter()
{
	isLocked = true;
	EnterCriticalSection(&Section);
}

void xrCriticalSection::Leave()
{
	isLocked = false;
	LeaveCriticalSection(&Section);
}

bool xrCriticalSection::TryLock()
{
	return isLocked;
}

xrCriticalSectionGuard::xrCriticalSectionGuard(xrCriticalSection& InSection)
	: Section(InSection)
{
	Section.Enter();
}

xrCriticalSectionGuard::~xrCriticalSectionGuard()
{
	Section.Leave();
}


#include <process.h>
#include <VersionHelpers.h>

using ThreadCall = HRESULT(WINAPI *)(HANDLE handle, PCWSTR name);

static bool Win10SupportNewThreadNameInit = false;
static ThreadCall pThreadCall = nullptr;

struct	THREAD_STARTUP
{
	thread_t* entry;
	char* name;
	void* args;
};

#pragma pack(push,8)
struct THREAD_NAME
{
	DWORD dwType;
	const char* szName;
	DWORD dwThreadID;
	DWORD dwFlags;
};

void thread_name(const char* name)
{
	if (IsWindows10OrGreater() && !Win10SupportNewThreadNameInit)
	{
		HMODULE KernelLib = GetModuleHandle("kernel32.dll");
		pThreadCall = (ThreadCall)GetProcAddress(KernelLib, "SetThreadDescription");
		Win10SupportNewThreadNameInit = true;
	}
	if (pThreadCall)
	{
		constexpr size_t cSize = 64;
		wchar_t wc[cSize];
		mbstowcs(wc, name, cSize);

		pThreadCall(GetCurrentThread(), wc);
	}
	else
	{
		THREAD_NAME tn;
		tn.dwType = 0x1000;
		tn.szName = name;
		tn.dwThreadID = DWORD(-1);
		tn.dwFlags = 0;
		__try
		{
			RaiseException(0x406D1388, 0, sizeof(tn) / sizeof(size_t), (size_t*)&tn);
		}
		__except (EXCEPTION_CONTINUE_EXECUTION)
		{
		}
	}
}
#pragma pack(pop)

void __cdecl thread_entry(void*	_params)
{
	// initialize
	THREAD_STARTUP* startup = (THREAD_STARTUP*)_params;
	thread_name(startup->name);
	thread_t* entry = startup->entry;
	void* arglist = startup->args;
	xr_delete(startup);
	_initialize_cpu_thread();

	// call
	entry(arglist);
}
HANDLE thread_spawn(thread_t* entry, const char* name, unsigned stack, void* arglist)
{
	THREAD_STARTUP* startup = new THREAD_STARTUP();
	startup->entry = entry;
	startup->name = (char*)name;
	startup->args = arglist;
	uintptr_t hThread = _beginthread(thread_entry, stack, startup);
	return reinterpret_cast<HANDLE> (hThread);
}