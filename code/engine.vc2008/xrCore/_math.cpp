#include "stdafx.h"
#pragma warning(disable: 4005)

#include <process.h>

// Initialized on startup
XRCORE_API Fmatrix Fidentity;
XRCORE_API Dmatrix Didentity;
XRCORE_API CRandom Random;

typedef struct _PROCESSOR_POWER_INFORMATION
{
	DWORD Number;
	DWORD MaxMhz;
	DWORD CurrentMhz;
	DWORD MhzLimit;
	DWORD MaxIdleState;
	DWORD CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

namespace FPU
{
	void initialize()
	{
		::Random.seed(u32(CPU::GetCLK() % (1i64 << 32i64)));
	}
};

namespace CPU
{
	XRCORE_API u64 qpc_freq;
	XRCORE_API u32 qpc_counter = 0;
	XRCORE_API processor_info Info;

	XRCORE_API u64 QPC() noexcept
	{
		u64 _dest;
		QueryPerformanceCounter(PLARGE_INTEGER(&_dest));
		qpc_counter++;
		return _dest;
	}
};

bool g_initialize_cpu_called = false;

//------------------------------------------------------------------------------------
void _initialize_cpu(void)
{
	const char* vendor;

	if (CPU::Info.isAmd)
		vendor = "AMD";
	else if (CPU::Info.isIntel)
		vendor = "Intel";
	else
		vendor = "VIA";

	Msg("* Vendor CPU: %s", vendor);

	Msg("* Detected CPU: %s", CPU::Info.modelName);

	string256	features;
	xr_strcpy(features, sizeof(features), "RDTSC");
	
	if (CPU::Info.hasFeature(CPUFeature::MMX))
		xr_strcat(features, ", MMX");

	if (CPU::Info.hasFeature(CPUFeature::AMD_3DNow))
		xr_strcat(features, ", 3DNow!");

	if (CPU::Info.hasFeature(CPUFeature::AMD_3DNowExt))
		xr_strcat(features, ", 3DNowExt!");

	if (CPU::Info.hasFeature(CPUFeature::SSE))
		xr_strcat(features, ", SSE");

	if (CPU::Info.hasFeature(CPUFeature::SSE2))
		xr_strcat(features, ", SSE2");

	if (CPU::Info.hasFeature(CPUFeature::SSE3))
		xr_strcat(features, ", SSE3");

	if (CPU::Info.hasFeature(CPUFeature::MWait))
		xr_strcat(features, ", MONITOR/MWAIT");

	if (CPU::Info.hasFeature(CPUFeature::SSSE3))
		xr_strcat(features, ", SSSE3");

	if (CPU::Info.hasFeature(CPUFeature::SSE41))
		xr_strcat(features, ", SSE4.1");

	if (CPU::Info.hasFeature(CPUFeature::SSE42))
		xr_strcat(features, ", SSE4.2");

	if (CPU::Info.hasFeature(CPUFeature::HT))
		xr_strcat(features, ", HTT");

	if (CPU::Info.hasFeature(CPUFeature::AVX))
		xr_strcat(features, ", AVX");
#ifdef __AVX__
	else Debug.do_exit(NULL, "X-Ray x64 using AVX anyway!");
#endif

	if (CPU::Info.hasFeature(CPUFeature::AVX2))
		xr_strcat(features, ", AVX2");

	if (CPU::Info.hasFeature(CPUFeature::SSE4a))
		xr_strcat(features, ", SSE4.a");

	if (CPU::Info.hasFeature(CPUFeature::MMXExt))
		xr_strcat(features, ", MMXExt");

	if (CPU::Info.hasFeature(CPUFeature::TM2))
		xr_strcat(features, ", TM2");

	if (CPU::Info.hasFeature(CPUFeature::AES))
		xr_strcat(features, ", AES");

	if (CPU::Info.hasFeature(CPUFeature::VMX))
		xr_strcat(features, ", VMX");

	if (CPU::Info.hasFeature(CPUFeature::EST))
		xr_strcat(features, ", EST");

	if (CPU::Info.hasFeature(CPUFeature::XFSR))
		xr_strcat(features, ", XFSR");

	Msg("* CPU features: %s", features);
	Msg("* CPU cores/threads: %d/%d \n", CPU::Info.n_cores, CPU::Info.n_threads);

	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	CPU::qpc_freq = Freq.QuadPart;

	Fidentity.identity();	// Identity matrix
	Didentity.identity();	// Identity matrix
	pvInitializeStatics();	// Lookup table for compressed normals
	FPU::initialize();
	_initialize_cpu_thread();

	g_initialize_cpu_called = true;
}

// per-thread initialization
#include <xmmintrin.h>
const int _MM_DENORMALS_ZERO = 0x0040;
const int _MM_FLUSH_ZERO = 0x8000;

inline void _mm_set_flush_zero_mode(u32 mode)
{
	_mm_setcsr((_mm_getcsr() & ~_MM_FLUSH_ZERO) | (mode));
}

inline void _mm_set_denormals_zero_mode(u32 mode)
{
	_mm_setcsr((_mm_getcsr() & ~_MM_DENORMALS_ZERO) | (mode));
}

static	bool _denormals_are_zero_supported = true;
void debug_on_thread_spawn();

void _initialize_cpu_thread()
{
	debug_on_thread_spawn();

	_mm_set_flush_zero_mode(_MM_FLUSH_ZERO);
	if (_denormals_are_zero_supported)
	{
		__try
		{
			_mm_set_denormals_zero_mode(_MM_DENORMALS_ZERO);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			_denormals_are_zero_supported = false;
		}
	}
}

bool float_equal(float a, float b)
{
	return _abs(a - b) < EPS_L;
}

unsigned long long SubtractTimes(const FILETIME one, const FILETIME two)
{ 
	LARGE_INTEGER a, b;
	a.LowPart = one.dwLowDateTime;
	a.HighPart = one.dwHighDateTime;

	b.LowPart = two.dwLowDateTime;
	b.HighPart = two.dwHighDateTime;

	return a.QuadPart - b.QuadPart;
}

bool processor_info::getCPULoad(double &val)
{
	FILETIME sysIdle, sysKernel, sysUser;
	// sysKernel include IdleTime
	if (GetSystemTimes(&sysIdle, &sysKernel, &sysUser) == 0) // GetSystemTimes func FAILED return value is zero;
		return false;

	if (prevSysIdle.dwLowDateTime != 0 && prevSysIdle.dwHighDateTime != 0)
	{
		DWORDLONG sysIdleDiff, sysKernelDiff, sysUserDiff;
		sysIdleDiff = SubtractTimes(sysIdle, prevSysIdle);
		sysKernelDiff = SubtractTimes(sysKernel, prevSysKernel);
		sysUserDiff = SubtractTimes(sysUser, prevSysUser);

		DWORDLONG sysTotal = sysKernelDiff + sysUserDiff;
		DWORDLONG kernelTotal = sysKernelDiff - sysIdleDiff; // kernelTime - IdleTime = kernelTime, because sysKernel include IdleTime

		if (sysTotal > 0) // sometimes kernelTime > idleTime
			val = (double)(((kernelTotal + sysUserDiff) * 100.0) / sysTotal);
	}

	prevSysIdle = sysIdle;
	prevSysKernel = sysKernel;
	prevSysUser = sysUser;

	return true;
}

#define NT_SUCCESS(Status) (((LONG)(Status)) >= 0)

float* processor_info::MTCPULoad()
{
	// get perfomance info by NtQuerySystemInformation()
	if (!NT_SUCCESS(m_pNtQuerySystemInformation(
		8,
		perfomanceInfo,
		sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * (ULONG)m_dwNumberOfProcessors,
		nullptr
	)))
	{
		Msg("Can't get NtQuerySystemInformation");
	}

	DWORD dwTickCount = GetTickCount();
	if (!m_dwCount) m_dwCount = dwTickCount;

	for (DWORD i = 0; i < m_dwNumberOfProcessors; i++)
	{
		SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* cpuPerfInfo = &perfomanceInfo[i];
		cpuPerfInfo->KernelTime.QuadPart -= cpuPerfInfo->IdleTime.QuadPart;

		fUsage[i] = 100.0f - 0.01f * (cpuPerfInfo->IdleTime.QuadPart - m_idleTime[i].QuadPart) / ((dwTickCount - m_dwCount));
		if (fUsage[i] < 0.0f) { fUsage[i] = 0.0f; }
		if (fUsage[i] > 100.0f) { fUsage[i] = 100.0f; }

		m_idleTime[i] = cpuPerfInfo->IdleTime;
	}

	m_dwCount = dwTickCount;
	return fUsage;
}

//#TODO: Return max value of float
float processor_info::CalcMPCPULoad(DWORD dwCPU)
{
	if (!m_pNtQuerySystemInformation)
		return FLT_MAX;

	if (dwCPU >= m_dwNumberOfProcessors)
		return FLT_MAX;


	DWORD dwTickCount = GetTickCount();
	//get standard timer tick count

		SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION info[MAX_CPU];

		if (SUCCEEDED(m_pNtQuerySystemInformation(SystemProcessorPerformanceInformation, &info, sizeof(info), nullptr)))
			//query CPU usage
		{
			if (m_idleTime[dwCPU].QuadPart)
				//ensure that this function was already called at least once
				//and we have the previous idle time value
			{
				m_fltCpuUsage[dwCPU] = 100.0f - 0.01f * (info[dwCPU].IdleTime.QuadPart - m_idleTime[dwCPU].QuadPart) / (dwTickCount - m_dwTickCount[dwCPU]);
				//calculate new CPU usage value by estimating amount of time
				//CPU was in idle during the last second

				//clip calculated CPU usage to [0-100] range to filter calculation non-ideality

				if (m_fltCpuUsage[dwCPU] < 0.0f)
					m_fltCpuUsage[dwCPU] = 0.0f;

				if (m_fltCpuUsage[dwCPU] > 100.0f)
					m_fltCpuUsage[dwCPU] = 100.0f;
			}

			m_idleTime[dwCPU] = info[dwCPU].IdleTime;
			//save new idle time for specified CPU
		}

	return m_fltCpuUsage[dwCPU];
}

xr_string xr_string::ToString(const Fvector& Value)
{
	string64 buf = { 0 };
	sprintf(buf, "[%f, %f, %f]", Value.x, Value.y, Value.z);

	return xr_string(buf);
}

xr_string xr_string::ToString(const Dvector& Value)
{
	string64 buf = { 0 };
	sprintf(buf, "[%f, %f, %f]", Value.x, Value.y, Value.z);

	return xr_string(buf);
}