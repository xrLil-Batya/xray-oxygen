#include "luajit\lua.hpp"
#include "luaopen.hpp"
#include "../xrCore/xrCore.h"

extern "C"
{
	int mar_clone(lua_State* L);
	int mar_decode(lua_State* L);
	int mar_encode(lua_State* L);

	int isWork(lua_State* L)
	{
		Log("Marshal is working!");
		return 0;
	}

}


static const struct luaL_Reg funcs[] =
{
	{ "encode", mar_encode },
	{ "decode", mar_decode },
	{ "clone",  mar_clone },
	{ "isWork",	isWork },
	{ 0,	    0 }
};

/* SCRIPT_API */ int lopen::marshal(lua_State *L)
{
#ifdef DEBUG
	Log("Marshal is working!");
#endif
//	lua_createtable(L, 0, 0);
//	lua_pushcfunction(L, isWork);
//	lua_pushstring(L, "mrsh");
//	lua_call(L, 1, 0);
	luaL_openlib(L, "mrsh", funcs, 0);
	return 1;
}