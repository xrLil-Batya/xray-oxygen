#include "stdafx.h"
#include "chimera.h"


#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("s",on)
void CChimera::script_register(lua_State *L)
{
	module(L)
	[
		class_<CChimera,CGameObject>("CChimera")
			.def(constructor<>())
	];
}
