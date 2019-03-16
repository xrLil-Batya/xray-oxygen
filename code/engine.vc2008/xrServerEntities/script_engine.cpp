////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "ai_space.h"
#include "object_factory.h"
#include "script_process.h"
#include "../xrScripts/lua_traceback.hpp"

#ifndef XRSE_FACTORY_EXPORTS
#	ifdef DEBUG
#		include "ai_debug.h"
extern Flags32 psAI_Flags;
#	endif
#endif

#include <luabind/luabind.hpp>
#include "../xrScripts/luaopen.hpp"

int CScriptEngine::lua_panic(lua_State* L) {
    ReportLuaError(L);
	return 0;
}

/*
int CScriptEngine::lua_panic(lua_State *L)
{
	
		print_output(L, "PANIC", LUA_ERRRUN);
		return (0);
}
*/

CScriptEngine::CScriptEngine()
{
	m_stack_level = 0;
	m_reload_modules = false;
	m_last_no_file_length = 0;
	*m_last_no_file = 0;
}

CScriptEngine::~CScriptEngine()
{
	while (!m_script_processes.empty())
		remove_script_process(m_script_processes.begin()->first);
}

void CScriptEngine::unload()
{
	lua_settop(lua(), m_stack_level);
	m_last_no_file_length = 0;
	*m_last_no_file = 0;
}

#ifdef LUABIND_NO_EXCEPTIONS
void CScriptEngine::lua_error(lua_State *L)
{
	/*
	print_output(L, "", LUA_ERRRUN);
#if !XRAY_EXCEPTIONS
	Debug.fatal(DEBUG_INFO, "LUA error: %s", lua_tostring(L, -1));
#else
	throw lua_tostring(L, -1);
#endif
*/

    ReportLuaError(L);
}
#endif

int  CScriptEngine::lua_pcall_failed(lua_State *L)
{
	/*
	print_output(L, "", LUA_ERRRUN);
#if !XRAY_EXCEPTIONS
	const char *error = lua_tostring(L, -1);
	Debug.fatal(DEBUG_INFO, "LUA error: %s", error ? error : get_traceback(L, 1));
#endif
	if (lua_isstring(L, -1))
		lua_pop(L, 1);
	return (LUA_ERRRUN);
	*/
    ReportLuaError(L);
    if (lua_isstring(L, -1))
        lua_pop(L, 1);
	return LUA_ERRRUN;

}

void CScriptEngine::ReportLuaError(lua_State* L)
{
    print_output(L, "[" __FUNCTION__ "]", LUA_ERRRUN);
    Msg("! [%s]: %s", __FUNCTION__, lua_isstring(L, -1) ? lua_tostring(L, -1) : "");
    FATAL("Lua critical error, check last log message");
}

#ifdef LUABIND_NO_EXCEPTIONS
#ifdef LUABIND_09
void lua_cast_failed(lua_State* L, const luabind::type_id& info)
#else
void  lua_cast_failed(lua_State* L, LUABIND_TYPE_INFO info)
#endif
{
	CScriptEngine::print_output(L, "[" __FUNCTION__ "]", LUA_ERRRUN); // ?
#ifdef LUABIND_09
	const char* info_name = info.name();
#else
	const char* info_name = info->name();
#endif
	Msg("!![%s] LUA error: cannot cast lua value to [%s]", __FUNCTION__, info_name);
}
#endif

	
/*
void lua_cast_failed(lua_State *L, LUABIND_TYPE_INFO info)
{
	CScriptEngine::print_output(L, "", LUA_ERRRUN);

	Debug.fatal(DEBUG_INFO, "LUA error: cannot cast lua value to %s", info->name());
}
*/
void CScriptEngine::setup_callbacks()
{
#pragma todo("FX to All: Restore it")
#if 0
#if !XRAY_EXCEPTIONS
	luabind::set_error_callback(CScriptEngine::lua_error);
#endif

#if !XRAY_EXCEPTIONS
	luabind::set_cast_failed_callback(lua_cast_failed);
#endif
#endif
	lua_atpanic(lua(), CScriptEngine::lua_panic);
}

#ifdef DEBUG
#	include "script_thread.h"
void CScriptEngine::lua_hook_call(lua_State *L, lua_Debug *dbg)
{
	if (ai().script_engine().current_thread())
		ai().script_engine().current_thread()->script_hook(L, dbg);
}
#endif

int auto_load(lua_State *L)
{
	if ((lua_gettop(L) < 2) || !lua_istable(L, 1) || !lua_isstring(L, 2)) {
		lua_pushnil(L);
		return		(1);
	}

	ai().script_engine().process_file_if_exists(lua_tostring(L, 2), false);
	lua_rawget(L, 1);
	return			(1);
}

typedef	xr_map<xr_string, xr_string>script_list_type;
static script_list_type xray_scripts;
void CScriptEngine::setup_auto_load()
{
	luaL_newmetatable(lua(), "XRAY_AutoLoadMetaTable");
	lua_pushstring(lua(), "__index");
	lua_pushcfunction(lua(), auto_load);
	lua_settable(lua(), -3);
	lua_pushstring(lua(), "_G");
	lua_gettable(lua(), LUA_GLOBALSINDEX);
	luaL_getmetatable(lua(), "XRAY_AutoLoadMetaTable");
	lua_setmetatable(lua(), -2);

	xray_scripts.clear();

	FS_FileSet fset;
	FS.file_list(fset, "$game_scripts$", FS_ListFiles, "*.script");

	for (auto &fit : fset)
	{
		string_path	fn1, fn2;
		_splitpath(fit.name.c_str(), 0, fn1, fn2, 0);

		FS.update_path(fn1, "$game_scripts$", fn1);
		xr_strconcat(fn1, fn1, fn2, ".script");

		xray_scripts.insert(std::make_pair(xr_string(fn2), xr_string(fn1)));
	}
}

extern void export_classes(lua_State *L);

void CScriptEngine::init()
{
	CScriptStorage::reinit();

	setup_callbacks();
	export_classes(lua());
	setup_auto_load();

	bool								save = m_reload_modules;
	m_reload_modules = true;
	process_file_if_exists("_g", false);
	m_reload_modules = save;

	register_script_classes();
	object_factory().register_script();

#ifdef XRGAME_EXPORTS
	load_common_scripts();
#endif
	m_stack_level = lua_gettop(lua());
}

void CScriptEngine::remove_script_process(const EScriptProcessors &process_id)
{
	CScriptProcessStorage::iterator	I = m_script_processes.find(process_id);
	if (I != m_script_processes.end()) 
	{
		xr_delete((*I).second);
		m_script_processes.erase(I);
	}
}

void CScriptEngine::load_common_scripts()
{
	string_path		S;
	FS.update_path(S, "$game_config$", "script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT(l_tpIniFile);
	if (!l_tpIniFile->section_exist("common")) {
		xr_delete(l_tpIniFile);
		return;
	}

	if (l_tpIniFile->line_exist("common", "script")) {
		LPCSTR			caScriptString = l_tpIniFile->r_string("common", "script");
		u32				n = _GetItemCount(caScriptString);
		string256		I;
		for (u32 i = 0; i<n; ++i)
		{
			process_file(_GetItem(caScriptString, i, I));
			xr_strcat(I, "_initialize");
			if (object("_G", I, LUA_TFUNCTION))
			{
				luabind::functor<void>	f;
				R_ASSERT(functor(I, f));
				f();
			}
		}
	}

	xr_delete(l_tpIniFile);
}

void CScriptEngine::process_file_if_exists(LPCSTR file_name, bool warn_if_not_exist)
{
	if (!*file_name)
		return;

	if (!m_reload_modules && namespace_loaded(file_name))
		return;

	script_list_type::iterator it = xray_scripts.find(xr_string(file_name));
	if (it != xray_scripts.end())
	{
		Msg("* loading script %s.script", file_name);
		m_reload_modules = false;

		load_file_into_namespace(it->second.c_str(), strcmp(file_name, "_g") == 0 ? "_G" : file_name);
		return;
	}

	if (warn_if_not_exist)
		Msg("Variable %s not found; No script by this name exists, either.", file_name);
}

void CScriptEngine::process_file(LPCSTR file_name)
{
	process_file_if_exists(file_name, true);
}

void CScriptEngine::process_file(LPCSTR file_name, bool reload_modules)
{
	m_reload_modules = reload_modules;
	process_file_if_exists(file_name, true);
	m_reload_modules = false;
}

void CScriptEngine::register_script_classes()
{/*
	string_path S;
	FS.update_path(S, "$game_config$", "script.ltx");
	CInifile *l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT(l_tpIniFile);

	if (!l_tpIniFile->section_exist("common")) 
	{
		xr_delete(l_tpIniFile);
		return;
	}

	m_class_registrators = READ_IF_EXISTS(l_tpIniFile, r_string, "common", "class_registrators", "");
	xr_delete(l_tpIniFile);

	u32 n = _GetItemCount(*m_class_registrators);
	string256 I;

	for (u32 i = 0; i<n; ++i) 
	{
		_GetItem(*m_class_registrators, i, I);
		luabind::functor<void>	result;
		if (!functor(I, result)) {
			script_log(eLuaMessageTypeError, "Cannot load class registrator %s!", I);
			continue;
		}
		result(const_cast<CObjectFactory*>(&object_factory()));
	}
	*/

	luabind::functor<void> result;
    if (!functor("class_registrator.register", result))
    {
        Msg("[%s] Cannot load class_registrator!", __FUNCTION__);
        FATAL("Can't load class_registrator, or error while registering script class");
    }
	result(const_cast<CObjectFactory*>(&object_factory()));
}

bool CScriptEngine::function_object(LPCSTR function_to_call, luabind::object &object, int type)
{
	if (!xr_strlen(function_to_call))
		return				(false);

	string256				name_space, function;

	parse_script_namespace(function_to_call, name_space, sizeof(name_space), function, sizeof(function));
	if (xr_strcmp(name_space, "_G")) {
		LPSTR				file_name = strchr(name_space, '.');
		if (!file_name)
			process_file(name_space);
		else {
			*file_name = 0;
			process_file(name_space);
			*file_name = '.';
		}
	}

	if (!this->object(name_space, function, type))
		return (false);

	luabind::object			lua_namespace = this->name_space(name_space);
	object = lua_namespace[function];
	return (true);
}

bool CScriptEngine::no_file_exists(LPCSTR file_name, u32 string_length)
{
	if (m_last_no_file_length != string_length)
		return (false);

	return (!memcmp(m_last_no_file, file_name, string_length * sizeof(char)));
}

void CScriptEngine::add_no_file(LPCSTR file_name, u32 string_length)
{
	m_last_no_file_length = string_length;
	std::memcpy(m_last_no_file, file_name, (string_length + 1) * sizeof(char));
}

void CScriptEngine::collect_all_garbage()
{
	lua_gc(lua(), LUA_GCCOLLECT, 0);
	lua_gc(lua(), LUA_GCCOLLECT, 0);
}