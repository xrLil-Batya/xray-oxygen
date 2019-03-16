#include "stdafx.h"
#include "holder_custom.h"
#include "actor.h"
#include <luabind/luabind.hpp>

bool CHolderCustom::attach_Actor(CGameObject* actor)
{
	m_owner						= actor;
	m_ownerActor				= smart_cast<CActor*>(actor);

	return true;
}

void CHolderCustom::detach_Actor()
{
	m_owner						= nullptr;
	m_ownerActor				= nullptr;
}

using namespace luabind;

#pragma optimize("gyts",on)
void CHolderCustom::script_register(lua_State *L)
{
	module(L)
		[
			class_<CHolderCustom>("holder")
				.def("engaged",												&CHolderCustom::Engaged)
				.def("Action",												&CHolderCustom::Action)
				.def("SetParamFv2", (void(CHolderCustom::*)(int, Fvector2))	&CHolderCustom::SetParam)
				.def("SetParam", (void(CHolderCustom::*)(int, Fvector))		&CHolderCustom::SetParam)
		];
}