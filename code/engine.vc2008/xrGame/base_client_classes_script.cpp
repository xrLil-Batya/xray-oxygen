////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes_script.cpp
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "base_client_classes.h"
#include "base_client_classes_wrappers.h"
#include "../xrEngine/feel_sound.h"
#include "../Include/xrRender/RenderVisual.h"
#include "../Include/xrRender/Kinematics.h"
#include "ai/stalker/ai_stalker.h"
#include "patrol_path.h"
#include "patrol_point.h"
#include <luabind/luabind.hpp>

using namespace luabind;

#pragma optimize("gyts",on)
void DLL_PureScript::script_register(lua_State *L)
{
	module(L)[
		class_<DLL_Pure, CDLL_PureWrapper>("DLL_Pure")
			.def(constructor<>())
			.def("_construct", &DLL_Pure::_construct, &CDLL_PureWrapper::_construct_static)
	];
}

void ISheduledScript::script_register(lua_State *L)
{
	module(L)[
		class_<ISheduled, CISheduledWrapper>("ISheduled")
	];
}

void IRenderableScript::script_register(lua_State *L)
{
	module(L)[
		class_<IRenderable, CIRenderableWrapper>("IRenderable")
	];
}

void ICollidableScript::script_register(lua_State *L)
{
	module(L)[
		class_<ICollidable>("ICollidable")
			.def(constructor<>())
	];
}

void CObjectScript::script_register(lua_State *L)
{
	module(L)[
		class_<CGameObject, bases<DLL_Pure, ISheduled, ICollidable, IRenderable>, CGameObjectWrapper>("CGameObject")
			.def(constructor<>())
			.def("_construct", &CGameObject::_construct, &CGameObjectWrapper::_construct_static)
			.def("Visual", &CGameObject::Visual)

			.def("net_Export", &CGameObject::net_Export, &CGameObjectWrapper::net_Export_static)
			.def("net_Spawn", &CGameObject::net_Spawn, &CGameObjectWrapper::net_Spawn_static)

			.def("use", &CGameObject::use, &CGameObjectWrapper::use_static)

			.def("getVisible", &CGameObject::getVisible)
			.def("setVisible", &CGameObject::setVisible)
			.def("getEnabled", &CGameObject::getEnabled)
			.def("setEnabled", &CGameObject::setEnabled)
	];
}

void IRender_VisualScript::script_register(lua_State *L)
{
	module(L)[
		class_<IRenderVisual>("IRender_Visual")
			//.def(constructor<>())
			.def("dcast_PKinematicsAnimated", &IRenderVisual::dcast_PKinematicsAnimated)
	];
}

void IKinematicsAnimated_PlayCycle(IKinematicsAnimated* sa, LPCSTR anim)
{
	sa->PlayCycle(anim);
}

void IKinematicsAnimatedScript::script_register(lua_State *L)
{
	module(L)[
		class_<IKinematicsAnimated>("IKinematicsAnimated")
			.def("PlayCycle", &IKinematicsAnimated_PlayCycle)
	];
}

void CBlendScript::script_register(lua_State *L)
{
	module(L)[
		class_<CBlend>("CBlend")
			//.def(constructor<>())
	];
}

LPCSTR CPatrolPointScript::getName(CPatrolPoint *pp) {
	return pp->m_name.c_str();
}
void CPatrolPointScript::setName(CPatrolPoint *pp, LPCSTR str) {
	pp->m_name = shared_str(str);
}

void CPatrolPointScript::script_register(lua_State *L)
{
	module(L)[
		class_<CPatrolPoint>("CPatrolPoint")
			.def(constructor<>())
			.def_readwrite("m_position", &CPatrolPoint::m_position)
			.def_readwrite("m_flags", &CPatrolPoint::m_flags)
			.def_readwrite("m_level_vertex_id", &CPatrolPoint::m_level_vertex_id)
			.def_readwrite("m_game_vertex_id", &CPatrolPoint::m_game_vertex_id)
			.property("m_name", &CPatrolPointScript::getName, &CPatrolPointScript::setName)
			.def("position", (CPatrolPoint& (CPatrolPoint::*) (Fvector)) (&CPatrolPoint::position))
	];
}

void CPatrolPathScript::script_register(lua_State *L)
{
	module(L)[
		class_<CPatrolPath>("CPatrolPath")
			.def(constructor<>())
			.def("add_point", &CPatrolPath::add_point)
			.def("point", (CPatrolPoint(CPatrolPath::*) (u32)) (&CPatrolPath::point))
			.def("add_vertex", &CPatrolPath::add_vertex)
	];
}
