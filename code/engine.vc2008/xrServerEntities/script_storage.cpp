////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.cpp
//	Created 	: 01.04.2004
//  Modified 	: 06.04.2017
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "script_storage.h"
#include "script_thread.h"
#include <stdarg.h>
#include "../FrayBuildConfig.hpp"
#include "../xrScripts/luaopen.hpp"
#include <luabind/luabind.hpp>

#ifdef XRSE_FACTORY_EXPORTS
#include "ai_space.h"
#include "script_engine.h"
#endif

const char*	file_header_old = "\
local function script_name() \
return \"%s\" \
end \
local this = {} \
%s this %s \
setmetatable(this, {__index = _G}) \
setfenv(1, this) \
		";

const char*	file_header_new = "\
local function script_name() \
return \"%s\" \
end \
local this = {} \
this._G = _G \
%s this %s \
setfenv(1, this) \
		";

const char*	file_header = 0;

#if !defined(ENGINE_BUILD) && defined(XRGAME_EXPORTS)
#	include "script_engine.h"
#	include "ai_space.h"
#	include "ai_debug.h"
#else
#	define NO_XRGAME_SCRIPT_ENGINE
#endif

CVMLua* CScriptStorage::luaVM = nullptr;
void xrScriptCrashHandler()
{
	Msg("Trying dump lua state");
	try
	{
		ai().script_engine().dump_state();
	}
	catch (...)
	{
		Msg("Can't dump script call stack - Engine corrupted");
	}
}

CScriptStorage::CScriptStorage()
{
	m_current_thread = 0;
    luaVM = nullptr;
}

CScriptStorage::~CScriptStorage()
{
    xr_delete(luaVM);
	Debug.set_crashhandler(nullptr);
}

void CScriptStorage::reinit()
{
	try
	{
		xr_delete(luaVM);
	}
	catch (...)
	{
		Msg("[ERROR] Error lua vm closed...");
	}

    luaVM = xr_new<CVMLua>();

	if (strstr(Core.Params, "-_g"))
		file_header = file_header_new;
	else
		file_header = file_header_old;

	//set our crash handler
	Debug.set_crashhandler(xrScriptCrashHandler);
}

void CScriptStorage::dump_state()
{
	static bool reentrantGuard = false;
	if (reentrantGuard) return;
	reentrantGuard = true;

	lua_State				*L = lua();
	lua_Debug				l_tDebugInfo;
	for (int i = 0; lua_getstack(L, i, &l_tDebugInfo); ++i) {
		lua_getinfo(L, "nSlu", &l_tDebugInfo);
		if (!l_tDebugInfo.name)
		{
			Msg("%2d : [%s] %s(%d)", i, l_tDebugInfo.what, l_tDebugInfo.short_src, l_tDebugInfo.currentline);
		}
		else if (!xr_strcmp(l_tDebugInfo.what, "C"))
		{
			Msg("%2d : [C  ] %s", i, l_tDebugInfo.name);
		}
		else
		{
			Msg("%2d : [%s] %s(%d) : %s", i, l_tDebugInfo.what, l_tDebugInfo.short_src, l_tDebugInfo.currentline, l_tDebugInfo.name);
		}
		Msg("\tLocals: ");
		const char *name = nullptr;
		int VarID = 1;
		while ((name = lua_getlocal(L, &l_tDebugInfo, VarID++)) != NULL)
		{
			LogVariable(L, name, 1, true);

			lua_pop(L, 1);  /* remove variable value */
		}
        m_dumpedObjList.clear();
		Msg("\tEnd");
	}
	reentrantGuard = false;
}

void CScriptStorage::LogTable(lua_State *l, const char* S, int level, int index /*= -1*/)
{
	if (!lua_istable(l, index))
		return;

	lua_pushnil(l);  /* first key */
	while (lua_next(l, index - 1) != 0) {
		char sname[256];
		char sFullName[256];
		xr_sprintf(sname, "%s", lua_tostring(l, index - 1));
		xr_sprintf(sFullName, "%s.%s", S, sname);
		LogVariable(l, sFullName, level + 1, false, index);

		lua_pop(l, 1);  /* removes `value'; keeps `key' for next iteration */
	}
}

void CScriptStorage::LogVariable(lua_State * l, const char* name, int level, bool bOpenTable, int index /*= -1*/)
{
	const char * type;
	int ntype = lua_type(l, index);
	type = lua_typename(l, ntype);

	char tabBuffer[32] = { 0 };
	memset(tabBuffer, '\t', level);

	char value[128];

	switch (ntype)
	{
	case LUA_TNUMBER:
		xr_sprintf(value, "%f", lua_tonumber(l, index));
		break;

	case LUA_TBOOLEAN:
		xr_sprintf(value, "%s", lua_toboolean(l, index) ? "true" : "false");
		break;

	case LUA_TSTRING:
		xr_sprintf(value, "%.127s", lua_tostring(l, index));
		break;

	case LUA_TTABLE:
		if (bOpenTable)
		{
			Msg("%s Table: %s", tabBuffer, name);
			LogTable(l, name, level + 1, index);
			return;
		}
		else
		{
			xr_sprintf(value, "[...]");
		}
		break;

	case LUA_TUSERDATA: 
	{
		luabind::detail::object_rep* obj = static_cast<luabind::detail::object_rep*>(lua_touserdata(l, index));

        // Skip already dumped object
        if (m_dumpedObjList.find(obj) != m_dumpedObjList.end()) return;
        m_dumpedObjList.insert(obj);
		luabind::detail::lua_reference& r = obj->get_lua_table();
		if (r.is_valid())
		{
			r.get(l);
			Msg("%s Userdata: %s", tabBuffer, name);
			LogTable(l, name, level + 1, index);
			lua_pop(l, 1); //Remove userobject
			return;
		}
		else
		{
            // Dump class and element pointer if available
            if (const luabind::detail::class_rep* objectClass = obj->crep())
            {
                xr_sprintf(value, "(%s): %p", objectClass->name(), obj->ptr());
            }
            else
            {
			    xr_strcpy(value, "[not available]");
            }
		}
	}
		break;

	default:
		xr_strcpy(value, "[not available]");
		break;
	}

	Msg("%s %s %s : %s", tabBuffer, type, name, value);
}

void CScriptStorage::ClearDumpedObjects()
{
    m_dumpedObjList.clear();
}

int __cdecl CScriptStorage::script_log(ScriptStorage::ELuaMessageType tLuaMessageType, const char* caFormat, ...)
{
	va_list marker;
	string2048 buf;
	ZeroMemory(&buf, sizeof(buf));
	va_start(marker, caFormat);
	int sz = vsnprintf(buf, sizeof(buf) - 1, caFormat, marker);
	if (sz != -1)
		luaVM->ScriptLog(tLuaMessageType, "%s", buf);
	va_end(marker);
	return 0;
}

bool CScriptStorage::parse_namespace(const char* caNamespaceName, char* b, u32 const b_size, char* c, u32 const c_size)
{
	*b = 0;
	*c = 0;
	
	string256			S2;
 	xr_strconcat		(S2,caNamespaceName);
 	char*			S	= S2;
	
	for (int i = 0;; ++i)
	{
		if (!xr_strlen(S))
		{
			script_log(ScriptStorage::eLuaMessageTypeError, "the namespace name %s is incorrect!", caNamespaceName);
			return		(false);
		}
		char*			S1 = strchr(S, '.');
		if (S1)
			*S1 = 0;

		if (i)
			xr_strcat(b, b_size, "{");
		xr_strcat(b, b_size, S);
		xr_strcat(b, b_size, "=");
		if (i)
			xr_strcat(c, c_size, "}");
		if (S1)
			S = ++S1;
		else
			break;
	}

	return (true);
}

bool CScriptStorage::load_buffer(lua_State *L, const char* caBuffer, size_t tSize, const char* caScriptName, const char* caNameSpaceName)
{
	int					l_iErrorCode;
	if (caNameSpaceName && xr_strcmp("_G", caNameSpaceName)) {
		string512		insert, a, b;

		if (!parse_namespace(caNameSpaceName, a, sizeof(a), b, sizeof(b)))
			return		(false);

		xr_sprintf(insert, file_header, caNameSpaceName, a, b);
		u32				str_len = xr_strlen(insert);
		size_t const	total_size = str_len + tSize;
		LPSTR			script = 0;
		bool dynamic_allocation = false;

		__try {
			if (total_size < 768 * 1024)
				script = (LPSTR)_alloca(total_size);
			else {
				script = (LPSTR)Memory.mem_alloc(total_size);
				dynamic_allocation = true;
			}
		}
		__except (GetExceptionCode() == STATUS_STACK_OVERFLOW)
		{
			int							errcode = _resetstkoflw();
			R_ASSERT2(errcode, "Could not reset the stack after \"Stack overflow\" exception!");
			script = (LPSTR)Memory.mem_alloc(total_size);
			dynamic_allocation = true;
		};

		xr_strcpy(script, total_size, insert);
		std::memcpy(script + str_len, caBuffer, u32(tSize));

		l_iErrorCode = luaL_loadbuffer(L, script, tSize + str_len, caScriptName);

		if (dynamic_allocation)
			xr_free(script);
	}
	else l_iErrorCode = luaL_loadbuffer(L, caBuffer, tSize, caScriptName);

	if (l_iErrorCode)
	{
#ifdef DEBUG
		print_output(L, caScriptName, l_iErrorCode);
#endif
		return			(false);
	}
	return				(true);
}

bool CScriptStorage::do_file(const char* caScriptName, const char* caNameSpaceName)
{
	int				start = lua_gettop(lua());
	string_path		l_caLuaFileName;
	IReader			*l_tpFileReader = FS.r_open(caScriptName);
	if (!l_tpFileReader) {
		script_log(eLuaMessageTypeError, "Cannot open file \"%s\"", caScriptName);
		return		(false);
	}
	xr_strconcat( l_caLuaFileName, "@", caScriptName);

	if (!load_buffer(lua(), static_cast<const char*>(l_tpFileReader->pointer()), (size_t)l_tpFileReader->length(), l_caLuaFileName, caNameSpaceName))
	{
		lua_settop(lua(), start);
		FS.r_close(l_tpFileReader);
		return		(false);
	}
	FS.r_close(l_tpFileReader);

	int errFuncId = -1;
	// because that's the first and the only call of the main chunk - there is no point to compile it
	int	l_iErrorCode = lua_pcall(lua(), 0, 0, (-1 == errFuncId) ? 0 : errFuncId);

	if (l_iErrorCode) 
	{
#ifdef DEBUG
		print_output(lua(), caScriptName, l_iErrorCode);
#endif
		lua_settop(lua(), start);
		return		(false);
	}

	return			(true);
}

bool CScriptStorage::load_file_into_namespace(const char* caScriptName, const char* caNamespaceName)
{
	int				start = lua_gettop(lua());
	if (!do_file(caScriptName, caNamespaceName)) {
		lua_settop(lua(), start);
		return		(false);
	}
	VERIFY(lua_gettop(lua()) == start);
	return			(true);
}

bool CScriptStorage::namespace_loaded(const char* N, bool remove_from_stack)
{
	int start = lua_gettop(lua());
	lua_pushstring(lua(), "_G");
	lua_rawget(lua(), LUA_GLOBALSINDEX);

	string256 S2;
 	xr_strcpy (S2,N);
 	char* S = S2;
	for (;;) {
		if (!xr_strlen(S))
		{
			VERIFY(lua_gettop(lua()) >= 1);
			lua_pop(lua(), 1);
			VERIFY(start == lua_gettop(lua()));
			return			(false);
		}
		LPSTR				S1 = strchr(S, '.');
		if (S1)
			*S1 = 0;
		lua_pushstring(lua(), S);
		lua_rawget(lua(), -2);
		if (lua_isnil(lua(), -1)) {
			VERIFY(lua_gettop(lua()) >= 2);
			lua_pop(lua(), 2);
			VERIFY(start == lua_gettop(lua()));
			return			(false);	//	there is no namespace!
		}
		else
			if (!lua_istable(lua(), -1)) {
				VERIFY(lua_gettop(lua()) >= 1);
				lua_pop(lua(), 1);
				VERIFY(start == lua_gettop(lua()));
				Debug.fatal(DEBUG_INFO, " Error : the namespace name %s is already being used by the non-table object!\n", S);
				return		(false);
			}
		lua_remove(lua(), -2);
		if (S1)
			S = ++S1;
		else
			break;
	}
	if (!remove_from_stack) {
		VERIFY(lua_gettop(lua()) == start + 1);
	}
	else {
		VERIFY(lua_gettop(lua()) >= 1);
		lua_pop(lua(), 1);
		VERIFY(lua_gettop(lua()) == start);
	}
	return					(true);
}

bool CScriptStorage::object(const char* identifier, int type)
{
	int						start = lua_gettop(lua());
	lua_pushnil(lua());
	while (lua_next(lua(), -2)) {

		if ((lua_type(lua(), -1) == type) && !xr_strcmp(identifier, lua_tostring(lua(), -2))) {
			VERIFY(lua_gettop(lua()) >= 3);
			lua_pop(lua(), 3);
			VERIFY(lua_gettop(lua()) == start - 1);
			return			(true);
		}
		lua_pop(lua(), 1);
	}
	VERIFY(lua_gettop(lua()) >= 1);
	lua_pop(lua(), 1);
	VERIFY(lua_gettop(lua()) == start - 1);
	return					(false);
}

bool CScriptStorage::object(const char* namespace_name, const char* identifier, int type)
{
	int						start = lua_gettop(lua());
	if (xr_strlen(namespace_name) && !namespace_loaded(namespace_name, false)) {
		VERIFY(lua_gettop(lua()) == start);
		return				(false);
	}
	bool					result = object(identifier, type);
	VERIFY(lua_gettop(lua()) == start);
	return					(result);
}

luabind::object CScriptStorage::name_space(const char* namespace_name)
{
	string256 S1;
 	xr_strcpy (S1,namespace_name);
	char* S = S1;
	luabind::object		lua_namespace = luabind::get_globals(lua());
	for (;;) {
		if (!xr_strlen(S))
			return		(lua_namespace);
		LPSTR			I = strchr(S, '.');
		if (!I)
			return		(lua_namespace[S]);
		*I = 0;
		lua_namespace = lua_namespace[S];
		S = I + 1;
	}
}

// FX: Added Error text to raii_guard
bool CScriptStorage::print_output(lua_State *L, const char* caScriptFileName, int iErrorCode, const char* caErrorText)
{
	return CVMLua::PrintOut(L, caScriptFileName, iErrorCode, caErrorText);
}

void CScriptStorage::print_error(lua_State *L, int iErrorCode)
{
	CVMLua::PrintError(L, iErrorCode);
}

#ifdef DEBUG
void CScriptStorage::flush_log()
{
}
#endif // DEBUG

int CScriptStorage::error_log(const char*	format, ...)
{
	va_list			marker;
	va_start(marker, format);

	const char*			S = "! [LUA][ERROR] ";
	LPSTR			S1;
	string4096		S2;
	xr_strcpy(S2, S);
	S1 = S2 + xr_strlen(S);

	int				result = vsprintf(S1, format, marker);
	va_end(marker);

	Msg("%s", S2);

	return			(result);
}