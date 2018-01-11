#include "stdafx.h"
#pragma hdrstop
#pragma warning(disable: 4005)

#include <process.h>
#include <powerbase.h>
#pragma comment (lib, "PowrProf.lib")
#include <mmsystem.h>

// Initialized on startup
XRCORE_API Fmatrix Fidentity;
XRCORE_API Dmatrix Didentity;
XRCORE_API CRandom Random;

typedef struct _PROCESSOR_POWER_INFORMATION 
{
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
 
namespace	FPU 
{
	XRCORE_API void 	m24		(void)	{
		_control87	( _PC_24,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
	}
	XRCORE_API void 	m24r	(void)	{
		_control87	( _PC_24,   MCW_PC );
		_control87	( _RC_NEAR, MCW_RC );
	}
	XRCORE_API void 	m53		(void)	{
		_control87	( _PC_53,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
	}
	XRCORE_API void 	m53r	(void)	{
		_control87	( _PC_53,   MCW_PC );
		_control87	( _RC_NEAR, MCW_RC );
	}
	XRCORE_API void 	m64		(void)	{
		_control87	( _PC_64,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
	}
	XRCORE_API void 	m64r	(void)	{
		_control87	( _PC_64,   MCW_PC );
		_control87	( _RC_NEAR, MCW_RC );
	}

	void initialize()				
	{
		 m24r();
		::Random.seed(u32(CPU::GetCLK() % (1i64 << 32i64)));
	}
};

namespace CPU 
{
    XRCORE_API u64 qpc_freq;
	XRCORE_API u32 qpc_counter = 0;
	XRCORE_API processor_info ID;

	XRCORE_API u64 QPC()
	{
		u64 _dest;
		QueryPerformanceCounter(PLARGE_INTEGER(&_dest));
		qpc_counter++;
		return _dest;
	}

    u64 getProcessorFrequencyGeneral()
    {
        u64 start, end;
        u32 dwStart, dwTest;

        dwTest = timeGetTime();
        do { dwStart = timeGetTime(); } while (dwTest == dwStart);
        start = GetCLK();
        while (timeGetTime() - dwStart < 1000);
        end = GetCLK();
        return end - start;
    }

    typedef struct _PROCESSOR_POWER_INFORMATION 
	{
		unsigned long Number;
        unsigned long MaxMhz;
        unsigned long CurrentMhz;
        unsigned long MhzLimit;
        unsigned long MaxIdleState;
        unsigned long CurrentIdleState;
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
};

//------------------------------------------------------------------------------------
void _initialize_cpu(void) 
{
	Msg("* Vendor CPU: %s", CPU::ID.vendor);
    Msg("* Detected CPU: %s", CPU::ID.modelName);

//	DUMP_PHASE;

	string256	features;	xr_strcpy(features,sizeof(features),"RDTSC");
    if (CPU::ID.hasFeature(CPUFeature::MMX))    xr_strcat(features,", MMX");
	if (CPU::ID.hasFeature(CPUFeature::AMD_3DNow)) xr_strcat(features, ", 3DNow!");
	if (CPU::ID.hasFeature(CPUFeature::AMD_3DNowExt)) xr_strcat(features, ", 3DNowExt!");
    if (CPU::ID.hasFeature(CPUFeature::SSE))    xr_strcat(features,", SSE");
    if (CPU::ID.hasFeature(CPUFeature::SSE2))   xr_strcat(features,", SSE2");
    if (CPU::ID.hasFeature(CPUFeature::SSE3))   xr_strcat(features,", SSE3");
    if (CPU::ID.hasFeature(CPUFeature::MWait))  xr_strcat(features, ", MONITOR/MWAIT");
    if (CPU::ID.hasFeature(CPUFeature::SSSE3))  xr_strcat(features,", SSSE3");
    if (CPU::ID.hasFeature(CPUFeature::SSE41))  xr_strcat(features,", SSE4.1");
    if (CPU::ID.hasFeature(CPUFeature::SSE42))  xr_strcat(features,", SSE4.2");
	if (CPU::ID.hasFeature(CPUFeature::HT))     xr_strcat(features, ", HTT");
	if (CPU::ID.hasFeature(CPUFeature::AVX))    xr_strcat(features, ", AVX");
#ifndef AI32_USE
	else Debug.do_exit("X-Ray x64 using AVX anyway!");
#endif
	if (CPU::ID.hasFeature(CPUFeature::AVX2))   xr_strcat(features, ", AVX2");
	if (CPU::ID.hasFeature(CPUFeature::SSE4a))  xr_strcat(features, ", SSE4.a");
	if (CPU::ID.hasFeature(CPUFeature::MMXExt)) xr_strcat(features, ", MMXExt");

	Msg("* CPU features: %s" , features );
	Msg("* CPU cores/threads: %d/%d \n", CPU::ID.n_cores, CPU::ID.n_threads);

    LARGE_INTEGER Freq;
    QueryPerformanceFrequency(&Freq);
    CPU::qpc_freq = Freq.QuadPart;

	Fidentity.identity		();	// Identity matrix
	Didentity.identity		();	// Identity matrix
	pvInitializeStatics		();	// Lookup table for compressed normals
	FPU::initialize			();
}
static const DWORD MS_VC_EXCEPTION = 0x406D1388;

// threading API 
#pragma pack(push,8)

typedef struct tagTHREADNAME_INFO 
{
	DWORD dwType;     // Must be 0x1000.
	LPCSTR szName;    // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void set_thread_name(DWORD dwThreadID, const char* threadName) 
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) 
	{
	}
}
void set_current_thread_name(const char* threadName) { set_thread_name(GetCurrentThreadId(), threadName); }

void set_thread_name(const char* threadName, std::thread& thread)
{
	DWORD threadId = ::GetThreadId(static_cast<HANDLE>(thread.native_handle()));
	set_thread_name(threadId, threadName);
}

void spline1(float t, Fvector *p, Fvector *ret)
{
	const float t2 = t * t;
	const float t3 = t2 * t;
	float m[4];

	ret->x = 0.0f;
	ret->y = 0.0f;
	ret->z = 0.0f;
	m[0] = (0.5f * ((-1.0f * t3) + (2.0f * t2) + (-1.0f * t)));
	m[1] = (0.5f * ((3.0f * t3) + (-5.0f * t2) + (0.0f * t) + 2.0f));
	m[2] = (0.5f * ((-3.0f * t3) + (4.0f * t2) + (1.0f * t)));
	m[3] = (0.5f * ((1.0f * t3) + (-1.0f * t2) + (0.0f * t)));

	for (u32 i = 0; i < 4; i++)
	{
		ret->x += p[i].x * m[i];
		ret->y += p[i].y * m[i];
		ret->z += p[i].z * m[i];
	}
}
