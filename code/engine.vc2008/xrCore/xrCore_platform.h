#pragma once


#if PLATFORM == _WINDOWS
	#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
	#define VC_EXTRALEAN				// Exclude rarely-used stuff from Windows headers
	// windows.h
	#ifndef _WIN32_WINNT
	#	define _WIN32_WINNT 0x0600
	#endif
#endif

#define NOGDICAPMASKS
//#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NODRAWTEXT
#define NOMEMMGR
//#define NOMETAFILE
#define NOSERVICE
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NOMCX
#define DOSWIN32
#define _WIN32_DCOM
#define NOMINMAX

#pragma warning(push)
#pragma warning(disable:4005)
#include <windows.h>

#if PLATFORM == _WINDOWS
	#include <windowsx.h>
#endif
#pragma warning(pop)

// Not available for all platforms
// Can't exclude macros - used in MS headers
//#ifdef FORMAT_MESSAGE_ALLOCATE_BUFFER
//	#undef FORMAT_MESSAGE_ALLOCATE_BUFFER
//#endif

#pragma warning (disable : 4512 )
