////////////////////////////////////////////////////////////////////////////
//	Module 		: lua_tools.h
//	Created 	: 29.07.2014
//	Author		: Alexander Petrov
////////////////////////////////////////////////////////////////////////////
//	Module 		: lua_traceback.h
//  Created 	: 12.07.2017
//	Author		: ForserX
//	Description : Lua functionality extension
////////////////////////////////////////////////////////////////////////////

#include "lua_tools.h"

void CLua_Traceback::get_lua_traceback(lua_State *L, int depth)
{
	static char  buffer[32768]; // global buffer
	int top = lua_gettop(L);
	// alpet: Lua traceback added
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushstring(L, "\t");
	lua_pushinteger(L, 1);

	m_traceback = "cannot get Lua traceback ";

	if (!lua_pcall(L, 2, 1, 0))
	{
		m_traceback = lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	lua_settop (L, top);
}