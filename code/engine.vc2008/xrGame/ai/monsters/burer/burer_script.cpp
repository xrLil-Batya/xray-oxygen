#include "stdafx.h"
#include "burer.h"


#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("s",on)
void CBurer::script_register(lua_State *L)
{
	module(L)
	[
		class_<CBurer,CGameObject>("CBurer")
			.def(constructor<>())
	];
}
