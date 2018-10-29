#include "stdafx.h"
#include "items/Scope.h"
#include "items/Silencer.h"
#include "items/GrenadeLauncher.h"
#include <luabind/luabind.hpp>

CScope::CScope	()
{
}

CScope::~CScope	() 
{
}

using namespace luabind;

#pragma optimize("s",on)
void CScope::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CScope,CGameObject>("CScope")
			.def(constructor<>()),
		
		class_<CSilencer,CGameObject>("CSilencer")
			.def(constructor<>()),

		class_<CGrenadeLauncher,CGameObject>("CGrenadeLauncher")
			.def(constructor<>())
	];
}
