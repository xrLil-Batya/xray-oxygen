////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_binder_object.h"
#include "script_game_object.h"

CScriptBinderObject::CScriptBinderObject	(CScriptGameObject *object)
{
	m_object		= object;
}

CScriptBinderObject::~CScriptBinderObject	()
{
#ifdef DEBUG
	if (m_object)
		Msg			("Destroying binded object %s",m_object->Name());
#endif
}

void CScriptBinderObject::reinit			()
{
}

void CScriptBinderObject::reload			(LPCSTR section)
{
}

bool CScriptBinderObject::net_Spawn			(SpawnType DC)
{
	return			(true);
}

void CScriptBinderObject::net_Destroy		()
{
}

void CScriptBinderObject::net_Import		(NET_Packet *net_packet)
{
}

void CScriptBinderObject::net_Export		(NET_Packet *net_packet)
{
}

void CScriptBinderObject::shedule_Update	(u32 time_delta)
{
}

void CScriptBinderObject::save				(NET_Packet *output_packet)
{
}

void CScriptBinderObject::load				(IReader	*input_packet)
{
}

bool CScriptBinderObject::net_SaveRelevant	()
{
	return		(false);
}

void CScriptBinderObject::net_Relcase		(CScriptGameObject *object)
{
}

#include "../xrScripts/export/script_export_space.h"
#include "script_binder_object_wrapper.h"
#include "xrServer_Objects_ALife.h"
#include <luabind/luabind.hpp>

using namespace luabind;

#pragma optimize("gyts",on)
void CScriptBinderObject::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptBinderObject,CScriptBinderObjectWrapper>("object_binder")
			.def_readonly("object",		&CScriptBinderObject::m_object)
			.def(						constructor<CScriptGameObject*>())
			.def("reinit",				&CScriptBinderObject::reinit,			&CScriptBinderObjectWrapper::reinit_static)
			.def("reload",				&CScriptBinderObject::reload,			&CScriptBinderObjectWrapper::reload_static)
			.def("net_spawn",			&CScriptBinderObject::net_Spawn,		&CScriptBinderObjectWrapper::net_Spawn_static)
			.def("net_destroy",			&CScriptBinderObject::net_Destroy,		&CScriptBinderObjectWrapper::net_Destroy_static)
			.def("net_import",			&CScriptBinderObject::net_Import,		&CScriptBinderObjectWrapper::net_Import_static)
			.def("net_export",			&CScriptBinderObject::net_Export,		&CScriptBinderObjectWrapper::net_Export_static)
			.def("update",				&CScriptBinderObject::shedule_Update,	&CScriptBinderObjectWrapper::shedule_Update_static)
			.def("save",				&CScriptBinderObject::save,				&CScriptBinderObjectWrapper::save_static)
			.def("load",				&CScriptBinderObject::load,				&CScriptBinderObjectWrapper::load_static)
			.def("net_save_relevant",	&CScriptBinderObject::net_SaveRelevant,	&CScriptBinderObjectWrapper::net_SaveRelevant_static)
			.def("net_Relcase",			&CScriptBinderObject::net_Relcase,		&CScriptBinderObjectWrapper::net_Relcase_static)
	];
}