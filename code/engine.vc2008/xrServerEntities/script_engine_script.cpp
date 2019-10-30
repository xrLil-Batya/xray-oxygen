////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_script.cpp
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator script engine export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "ai_space.h"
#include "../xrScripts/import_ses.hpp"

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

using namespace luabind;

void ErrorLog(LPCSTR caMessage)
{
	ai().script_engine().error_log("%s",caMessage);
	R_ASSERT2(false, caMessage);
}

void FlushLogs()
{
#ifdef DEBUG
	xrLogger::FlushLog();
	ai().script_engine().flush_log();
#endif // DEBUG
}

void verify_if_thread_is_running()
{
	THROW2	(ai().script_engine().current_thread(),"coroutine.yield() is called outside the LUA thread!");
}

bool is_editor()
{
#ifdef XRGAME_EXPORTS
	return		(false);
#else
	return		(true);
#endif
}

#ifdef XRGAME_EXPORTS
CRenderDevice *get_device()
{
	return		(&Device);
}
#endif

void Lua_DebugBreak()
{
    DebugBreak();
}

void Lua_CrashGame()
{
	Msg("* LUA: CrashGame invoked, this is a test crash, it's not a bug");

	int* pSuicidePtr = nullptr;
	*pSuicidePtr = 42;
}

void Lua_Object_Info()
{
    CScriptEngine& scriptEngine = ai().script_engine();
    scriptEngine.LogVariable(scriptEngine.lua(), "[lua_object_info]", 1, true, 1);
    scriptEngine.ClearDumpedObjects();
}

void prefetch_module(LPCSTR file_name)
{
	ai().script_engine().process_file(file_name);
}

struct profile_timer_script 
{
	u64 m_start_cpu_tick_count;
	u64 m_accumulator;
	u64 m_count;
	int m_recurse_mark;
	
	IC profile_timer_script()
	{
		m_start_cpu_tick_count	= 0;
		m_accumulator			= 0;
		m_count					= 0;
		m_recurse_mark			= 0;
	}

	IC profile_timer_script	(const profile_timer_script &profile_timer)
	{
		*this = profile_timer;
	}

	IC profile_timer_script& operator=(const profile_timer_script &profile_timer)
	{
		m_start_cpu_tick_count	= profile_timer.m_start_cpu_tick_count;
		m_accumulator			= profile_timer.m_accumulator;
		m_count					= profile_timer.m_count;
		m_recurse_mark			= profile_timer.m_recurse_mark;
		return (*this);
	}

	ICF bool operator<(const profile_timer_script &profile_timer) const
	{
		return (m_accumulator < profile_timer.m_accumulator);
	}

	IC void start()
	{
		if (m_recurse_mark) 
		{
			++m_recurse_mark;
			return;
		}

		++m_recurse_mark;
		++m_count;
		m_start_cpu_tick_count	= CPU::GetCLK();
	}

	IC void stop()
	{
		if (!m_recurse_mark)
		{
			--m_recurse_mark;
		
			if (m_recurse_mark)
				return;
		
			u64 finish = CPU::GetCLK();
			if (finish > m_start_cpu_tick_count)
				m_accumulator += finish - m_start_cpu_tick_count;
		}
	}

	IC float time() const
	{
		float					result = (float(double(m_accumulator)/double(CPU::clk_per_second))*1000000.f);
		return					(result);
	}
};

IC	profile_timer_script	operator+	(const profile_timer_script &portion0, const profile_timer_script &portion1)
{
	profile_timer_script	result;
	result.m_accumulator	= portion0.m_accumulator + portion1.m_accumulator;
	result.m_count			= portion0.m_count + portion1.m_count;
	return					(result);
}

#ifdef XRGAME_EXPORTS
ICF	u32	script_time_global	()	{ return Device.dwTimeGlobal; }
ICF	u32	script_time_global_async	()	{ return Device.TimerAsync_MMT(); }
#else
ICF	u32	script_time_global	()	{ return NULL; }
ICF	u32	script_time_global_async	()	{ return NULL; }
#endif+

#ifdef XRGAME_EXPORTS
static bool is_enough_address_space_available_impl()
{
	ENGINE_API bool is_enough_address_space_available();
	return is_enough_address_space_available( );
}
#endif // #ifdef XRGAME_EXPORTS

#pragma optimize("s",on)
void CScriptEngine::script_register(lua_State *L)
{
	module(L)[
		class_<profile_timer_script>("profile_timer")
			.def(constructor<>())
			.def(constructor<profile_timer_script&>())
			.def(const_self + profile_timer_script())
			.def(const_self < profile_timer_script())
			.def(tostring(self))
			.def("start",&profile_timer_script::start)
			.def("stop",&profile_timer_script::stop)
			.def("time",&profile_timer_script::time)
	];
	
    function    (L, "to_log",							import_ses::LuaLog);
    function    (L, "log",								import_ses::LuaLog);
	function	(L,	"error_log",						ErrorLog);
	function	(L,	"flush",							FlushLogs);
	function	(L,	"prefetch",							prefetch_module);
	function	(L,	"verify_if_thread_is_running",		verify_if_thread_is_running);
	function	(L,	"editor",							is_editor);
	function	(L, "user_name",						import_ses::user_name);
	function	(L, "time_global",						script_time_global);
	function	(L, "time_global_async",				script_time_global_async);
    function    (L, "debuggerTrigger",                  Lua_DebugBreak);
    function    (L, "crashGame",						Lua_CrashGame);
    function    (L, "lua_object_info",                  Lua_Object_Info);
	//function	(L, "LoadModule", load_modules);
#ifdef XRGAME_EXPORTS
	function	(L,	"device",							get_device);
	function	(L,	"is_enough_address_space_available",is_enough_address_space_available_impl);
#endif // #ifdef XRGAME_EXPORTS
}
