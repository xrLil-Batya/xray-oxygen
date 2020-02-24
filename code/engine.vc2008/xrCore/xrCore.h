#pragma once
#pragma warning (disable : 4530 )		// C++ vector(985)

#define ENGINE_VERSION "January 2020"

#ifndef DEBUG
#	define MASTER_GOLD
#endif // DEBUG

#include "xrCore_platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <typeinfo>
#include <cinttypes>

// inline control - redefine to use compiler's heuristics ONLY
// it seems "IC" is misused in many places which cause code-bloat
// ...and VC7.1 really don't miss opportunities for inline :)
#define _inline			inline
#define __inline		inline
#define IC				inline
#define ICF				__forceinline			// !!! this should be used only in critical places found by PROFILER
#define ICN				__declspec(noinline)

#pragma inline_depth	( 254 )
#pragma inline_recursion( on )

// Warnings
#pragma warning (disable : 4251 )		// object needs DLL interface
#pragma warning (disable : 4201 )		// nonstandard extension used : nameless struct/union
#pragma warning (disable : 4100 )		// unreferenced formal parameter
#pragma warning (disable : 4127 )		// conditional expression is constant
#pragma warning (disable : 4345 )
#pragma warning (disable : 4714 )		// __forceinline not inlined
#ifndef DEBUG
#pragma warning (disable : 4189 )		//  local variable is initialized but not refenced
#endif									//	frequently in release code due to large amount of VERIFY

// stl
#pragma warning (push)
#pragma warning (disable:4702)
#include <algorithm>
#include <limits>
#include <vector>
#include <queue>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <utility>
#include <string>

#include <unordered_map>
#include <unordered_set>

#pragma warning (pop)
#pragma warning (disable : 4100 )		// unreferenced formal parameter

// Our headers
#ifdef XRCORE_EXPORTS
#	define XRCORE_API __declspec(dllexport)
#else
#	define XRCORE_API __declspec(dllimport)
#endif

#include "_types.h"
#include "RingBuffer.h"
#include "thread_utils.h"
#include "xrDebug.h"
#include "xrMemory.h"
#include "_std_extensions.h"
#include "_stl_extensions.h"
#include "xrProfiling.h"
#include "vector.h"

#if PLATFORM == _WINDOWS
	#include "Platform/Windows/xrWindowsPlatformUtils.h"
#elif PLATFORM == _XBOX_ONE
	#include "Platform/Xbox/xrXboxPlatformUtils.h"
	#include "Platform/Xbox/xrXboxAdvApi.h"
#endif

#include "clsid.h"

#include "xrSharedMem.h"
#include "xrString.h"
#include "xr_resource.h"
#include "rt_compressor.h"
#include "xr_shared.h"
#include "ScopeHandle.h"

#ifdef DEBUG
#include "dump_string.h"
#endif

// stl ext
struct XRCORE_API xr_rtoken
{
	shared_str	name;
	int	   		id;

			xr_rtoken	(const char* _nm, int _id): name(_nm), id(_id) {}
	void	rename		(const char* _nm) { name = _nm; }
	bool	equal		(const char* _nm) { return (0 == xr_strcmp(*name, _nm)); }
};

#pragma pack (push,1)
struct XRCORE_API xr_shortcut
{
	enum 
	{
		flShift = 0x20,
		flCtrl  = 0x40,
		flAlt   = 0x80,
	};
	union 
	{
		struct 
		{
			u8	 	key;
			Flags8	ext;
		};
		u16 hotkey;
	};
			xr_shortcut(u8 k, BOOL a, BOOL c, BOOL s) :key(k) { ext.assign(u8((a ? flAlt : 0) | (c ? flCtrl : 0) | (s ? flShift : 0))); }
			xr_shortcut() { ext.zero(); key = 0; }
	bool	similar(const xr_shortcut& v) const { return (ext.flags == v.ext.flags) && (key == v.key); }
};
#pragma pack (pop)

using RStringVec = xr_vector<shared_str>;
using RStringSet = xr_set<shared_str>;
using RTokenVec = xr_vector<xr_rtoken>;

#include "FS.h"
#include "log.h"
#include "xr_trims.h"
#include "xr_ini.h"
#include "LocatorAPI.h"
#include "FileSystem.h"
#include "FTimer.h"
#include "intrusive_ptr.h"
#include "net_utils.h"

// destructor
template <class T>
class destructor
{
	T* ptr;
public:
	destructor(T* p) { ptr = p; }
	~destructor() { xr_delete(ptr); }
	IC T& operator() ()
	{
		return *ptr;
	}
};

template <class T>
bool reclaim(xr_vector<T*>& vec, const T* pTypePtr)
{
	u32 Iter = 0;
	for (T* pType: vec)
	{
		if (pType == pTypePtr)	
		{ 
			vec.erase(vec.begin() + Iter); 
			return true; 
		}
		Iter++;
	}
	
	return false;
}

// ********************************************** The Core definition
struct XRCORE_API xrCore
{
	bool		PluginMode;
	bool		bSpectreEnabled;

	DWORD		dwFrame;

	string64	ApplicationName;
	string64	UserName;
	string64	CompName;
	string_path	ApplicationPath;
	string_path	WorkingPath;
	string1024	Params;

	void		_initialize(const char* ApplicationName, xrLogger::LogCallback cb = 0, BOOL init_fs = TRUE, const char* fs_fname = 0);
	void		_destroy();
	IC	void	SetPluginMode() { PluginMode = true; }

	HMODULE LoadModule(LPCSTR ModuleName, bool bAllowFail = false);
};

extern XRCORE_API xrCore Core;
extern XRCORE_API bool   gModulesLoaded;

#include "XMLCore\xrXMLParser.h"

#ifndef SPECTRE
	#include "tbb/concurrent_unordered_map.h"
	#include "tbb/concurrent_vector.h"

	template<typename Key, typename Value>
	using xr_concurrent_unordered_map = tbb::concurrent_unordered_map < Key, Value, tbb::tbb_hash< Key >, std::equal_to< Key >, xalloc< std::pair< const Key, Value > > >;

	template<typename Key, typename Value>
	using xr_concurrent_unordered_multimap = tbb::concurrent_unordered_multimap < Key, Value, tbb::tbb_hash< Key >, std::equal_to< Key >, xalloc< std::pair< const Key, Value > > >;

	template<typename ElemType>
	using xr_concurrent_vector = tbb::concurrent_vector < ElemType, xalloc< ElemType >>;
#endif