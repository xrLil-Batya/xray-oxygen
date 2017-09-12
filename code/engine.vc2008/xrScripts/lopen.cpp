// file:	lopen.cpp
// func:	Open lua modules-namespace
// author:	ForserX

#include "luajit\lua.hpp"
#include "luaopen.hpp"
#include "../xrCore/xrCore.h"

extern "C"
{
	extern const struct luaL_Reg *funcs;
	int luaopen_LuaXML_lib(lua_State* L);
}

/* SCRIPT_API */ int lopen::marshal(lua_State *L)
{
	luaL_openlib(L, "mrsh", funcs, 0);
	luaopen_LuaXML_lib(L); // Чтоб не пересобирать xrGame 
	return 1;
}