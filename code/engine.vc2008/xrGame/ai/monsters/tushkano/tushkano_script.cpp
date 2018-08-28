#include "stdafx.h"
#include "tushkano.h"


#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("s",on)
void CTushkano::script_register(lua_State *L)
{
	module(L)
		[
			class_<CTushkano,CGameObject>("CTushkano")
			.def(constructor<>())
		];
}
