#include "stdafx.h"
#include "Explosive.h"


#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("gyts",on)
void CExplosive::script_register(lua_State *L)
{
	module(L)
	[
		class_<CExplosive>("explosive")
			.def("explode",					(&CExplosive::Explode))
	];
}

