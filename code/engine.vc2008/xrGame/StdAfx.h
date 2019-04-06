#pragma once

#include "xrGame.h"

#pragma warning(disable:4995)
#include "../xrEngine/stdafx.h"
#include <d3d9types.h>
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

#include "../Include/xrRender/WallMarkArray.h"
#include "../Include/xrRender/FactoryPtr.h"

// Forward luaState
struct lua_State;