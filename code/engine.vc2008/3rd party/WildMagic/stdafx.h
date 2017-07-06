//----------------------------------------------------
// file: stdafx.h
//----------------------------------------------------
#pragma once
// Std C++ headers
//#include <fastmath.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <process.h>
//#include <utime.h>

// DirectX headers
#include <d3d9.h>
#ifdef MIXED
#define	USE_MEMORY_MONITOR
#endif
#ifdef RELEASE
#	undef _CPPUNWIND
#endif
#include "..\..\xrCore\xrCore.h"

#include <dinput.h>