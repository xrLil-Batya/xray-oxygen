//////////////////////////////////////////////////////////////
// Desc		: Windows miscellaneous functions
// Author	: Giperion
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "xrWindowsPlatformUtils.h"
#include <mmsystem.h>
#include <powerbase.h>
#include <VersionHelpers.h>
#include "_math.h"

#pragma comment (lib, "PowrProf.lib")


XRCORE_API xrWindowsPlatformUtils PlatformUtils;

LPCSTR xrWindowsPlatformUtils::GetName()
{
	return "Windows-x64";
}

int xrWindowsPlatformUtils::ShowCursor(bool bShowCursor)
{
	return ::ShowCursor(bShowCursor);
}

void xrWindowsPlatformUtils::GetUsername(string64& OutUsername)
{
	DWORD sizeOfUsername = sizeof(OutUsername);
	if (!GetUserName(OutUsername, &sizeOfUsername))
	{
		LPCSTR DefaultName = "Unknown";
		memcpy(OutUsername, DefaultName, xr_strlen(DefaultName));
	}
}

void xrWindowsPlatformUtils::GetComputerName(string64& OutComputer)
{
	DWORD sizeOfComputer = sizeof(OutComputer);
	if (!::GetComputerName(OutComputer, &sizeOfComputer))
	{
		LPCSTR DefaultName = "WinPC";
		memcpy(OutComputer, DefaultName, xr_strlen(DefaultName));
	}
}

u64 getProcessorFrequencyGeneral()
{
	u64 start, end;
	u32 dwStart, dwTest;

	dwTest = timeGetTime();
	do { dwStart = timeGetTime(); } while (dwTest == dwStart);
	start = CPU::GetCLK();
	while (timeGetTime() - dwStart < 1000);
	end = CPU::GetCLK();
	return end - start;
}

typedef struct _PROCESSOR_POWER_INFORMATION
{
	ULONG Number;
	ULONG MaxMhz;
	ULONG CurrentMhz;
	ULONG MhzLimit;
	ULONG MaxIdleState;
	ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

u64 getProcessorFrequency(u32 logicalProcessorCount)
{
	PROCESSOR_POWER_INFORMATION* pInfo = reinterpret_cast<PROCESSOR_POWER_INFORMATION*> (alloca(sizeof(PROCESSOR_POWER_INFORMATION) * logicalProcessorCount));
	LONG retCode = CallNtPowerInformation(ProcessorInformation, nullptr, 0, pInfo, sizeof(PROCESSOR_POWER_INFORMATION) * logicalProcessorCount);
	if (retCode != 0x0l)
	{
		return getProcessorFrequencyGeneral();
	}
	return pInfo->MhzLimit * u64(1000000);
}

u64 xrWindowsPlatformUtils::GetProcessorFrequency()
{
	return getProcessorFrequency(CPU::Info.n_threads);
}

using ThreadCall = HRESULT(WINAPI *)(HANDLE handle, PCWSTR name);

static bool Win10SupportNewThreadNameInit = false;
static ThreadCall pThreadCall = nullptr;

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

void xrWindowsPlatformUtils::SetCurrentThreadName(const string128& ThreadName)
{
	thread_name(ThreadName);
}
