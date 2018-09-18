#include "stdafx.h"
#include "cat.h"


#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("s",on)
void CCat::script_register(lua_State *L)
{
	module(L)
	[
		class_<CCat,CGameObject>("CCat")
			.def(constructor<>())
	];
}
