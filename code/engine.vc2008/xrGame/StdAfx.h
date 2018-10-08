#pragma once

#include "xrGame.h"

#pragma warning(disable:4995)
#include "../xrEngine/stdafx.h"
#include <DPlay/dplay8.h>
#include <dx/dinput.h>
#pragma warning(default:4995)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )
#pragma warning(disable:4505)

#if XRAY_EXCEPTIONS
#	define	THROW(expr)				do {if (!(expr)) {string4096	assertion_info; ::Debug.gather_info(_TRE(#expr),   0,   0,0,DEBUG_INFO,assertion_info); throw assertion_info;}} while(0)
#	define	THROW2(expr,msg0)		do {if (!(expr)) {string4096	assertion_info; ::Debug.gather_info(_TRE(#expr),msg0,   0,0,DEBUG_INFO,assertion_info); throw assertion_info;}} while(0)
#	define	THROW3(expr,msg0,msg1)	do {if (!(expr)) {string4096	assertion_info; ::Debug.gather_info(_TRE(#expr),msg0,msg1,0,DEBUG_INFO,assertion_info); throw assertion_info;}} while(0)
#else
#	define	THROW					VERIFY
#	define	THROW2					VERIFY2
#	define	THROW3					VERIFY3
#endif
#include "../xrEngine/gamefont.h"
#include "../xrEngine/IGame_Level.h"
#include "../xrEngine/bone.h"
#include "../xrEngine/xr_object.h"
#include "../xrphysics/xrphysics.h"
#include "smart_cast.h"
#include "GamePersistent.h"

// Forward luaState
struct lua_State;

#ifndef D3DCOLOR_XRGB
	#define D3DCOLOR_ARGB(a,r,g,b) \
		((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
	#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
	#define D3DCOLOR_XRGB(r,g,b)   D3DCOLOR_ARGB(0xff,r,g,b)
#endif

/*
extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
};

#pragma warning(push)
#pragma warning(disable:4995)
#include <luabind/luabind.hpp>
#pragma warning(pop)

#include <luabind/object.hpp>
#include <luabind/functor.hpp>
#include <luabind/operator.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/iterator_policy.hpp>
*/