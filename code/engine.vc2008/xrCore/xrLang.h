/////////////////////////////////////////////////
// Author : ForserX
// Desc   : bases templates for C++ classes
/////////////////////////////////////////////////
// Oxygen Engine 2.0 - 2016-2019
/////////////////////////////////////////////////
#pragma once

// Maybe use C++11? 
#if _MSC_VER < 1911
#define string_view xr_string&
#else
#include <string_view>
#endif

#ifdef WIN32
// SSE3 Optimizations
#pragma intrinsic(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcat, pow, strlen, memset)

#include <time.h>
// work-around dumb borland compiler
#define ALIGN(a) __declspec(align(a))
#include <sys\utime.h>
#endif

// Only MT Build!
#pragma warning(disable:4996)
#if !defined(_MT)
	// multithreading disabled
#error Please enable multi-threaded library...
#endif

// Check if user included some files, that a prohibited
#ifdef _MUTEX_
#error <mutex> file is prohibited, please use xrCriticalSection and xrCriticalSectionGuard instead
#endif

// Ban std::thread also
#ifdef _THREAD_
#error <thread> is prohibited, please use TBB Task, or _beginthreadex
#endif

#ifdef _CHARCONV_
#error <charconv> is prohibited, it doesn't exist on all supported platforms
#endif

#define xr_interface __interface
#define ALIGN(a) __declspec(align(a))

class TNonCopyable
{
public:
    TNonCopyable() = default;
    ~TNonCopyable() = default;

private:
    TNonCopyable(const TNonCopyable&) = delete;
    TNonCopyable& operator=(const TNonCopyable&) = delete;
};

#define TMakeSingleton(T)				\
	public:                             \
	T(T const&) = delete;               \
	void operator=(T const&) = delete;  \
	static T& GetInstance()             \
	{                                   \
		static T Instance;              \
		return Instance;                \
	}