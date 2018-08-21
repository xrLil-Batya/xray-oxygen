////////////////////////////////////////////////////////////////////////////
//	Module 		: script_thread.cpp
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script thread class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <luabind/lua_include.hpp>
#include "script_engine.h"
#include "script_thread.h"
#include "ai_space.h"

#define LUABIND_HAS_BUGS_WITH_LUA_THREADS

const LPCSTR main_function = "console_command_run_string_main_thread_function";

CScriptThread::CScriptThread(LPCSTR caNamespaceName, bool do_string, bool reload)
{
	m_virtual_machine		= nullptr;
	m_active				= false;

	try {
		string256			S;
		if (!do_string) {
			m_script_name	= caNamespaceName;
			ai().script_engine().process_file(caNamespaceName,reload);
		}
		else {
			m_script_name	= "console command";
			xr_sprintf			(S,"function %s()\n%s\nend\n",main_function,caNamespaceName);
			int				l_iErrorCode = luaL_loadbuffer(ai().script_engine().lua(),S,xr_strlen(S),"@console_command");
			if (!l_iErrorCode) 
			{
				l_iErrorCode = lua_pcall(ai().script_engine().lua(),0,0,0);
				if (l_iErrorCode) {
					ai().script_engine().print_output	(ai().script_engine().lua(),*m_script_name,l_iErrorCode);
					return;
				}
			}
			else 
			{
				ai().script_engine().print_output		(ai().script_engine().lua(),*m_script_name,l_iErrorCode);
				return;
			}
		}

		m_virtual_machine	= lua_newthread(ai().script_engine().lua());
		VERIFY2				(lua(),"Cannot create new Lua thread");

		if (!do_string)
			xr_sprintf			(S,"%s.main()",caNamespaceName);
		else
			xr_sprintf			(S,"%s()",main_function);

		if (!ai().script_engine().load_buffer(lua(),S,xr_strlen(S),"@_thread_main"))
			return;

		m_active			= true;
	}
	catch(...) {
		m_active			= false;
	}
}

CScriptThread::~CScriptThread()
{
#ifndef LUABIND_HAS_BUGS_WITH_LUA_THREADS
	try 
	{
		luaL_unref(ai().script_engine().lua(),LUA_REGISTRYINDEX,m_thread_reference);
	}
	catch(...) 
	{
	}
#endif
}

bool CScriptThread::update()
{
	R_ASSERT2(m_active, "Cannot resume dead Lua thread!");

	try 
	{
		ai().script_engine().current_thread(this);

		int l_iErrorCode = lua_resume(lua(), 0);

		if (l_iErrorCode && (l_iErrorCode != LUA_YIELD))
		{
			ai().script_engine().print_output(lua(), *script_name(), l_iErrorCode);
			m_active = false;
		}
		else {
			if (l_iErrorCode != LUA_YIELD)
			{
#ifdef DEBUG
				if (m_current_stack_level)
				{
					ai().script_engine().print_output(lua(), *script_name(), l_iErrorCode);
				}
#endif // DEBUG
				m_active = false;
			}
			else VERIFY2(!lua_gettop(lua()), "Do not pass any value to coroutine.yield()!");
		}

		ai().script_engine().current_thread(nullptr);
	}
	catch (...) {
		ai().script_engine().current_thread(nullptr);
		m_active = false;
	}
	return (m_active);
}
