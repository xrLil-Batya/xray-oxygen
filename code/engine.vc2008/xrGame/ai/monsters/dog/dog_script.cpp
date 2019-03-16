#include "stdafx.h"
#include "dog.h"


#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("s",on)
void CAI_Dog::script_register(lua_State *L)
{
	module(L)
	[
		class_<CAI_Dog,CGameObject>("CAI_Dog")
			.def(constructor<>())
	];
}
