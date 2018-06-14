// Singleton LuaVM 
// ForserX 27.05.2018 for xrOxygen
#include "luajit/lua.hpp"
#include <luabind/luabind.hpp>

#include "../xrCore/xrCore.h"
#include "VMLua.h"
#include "luaopen.hpp"

static void* __cdecl luabind_allocator(luabind::memory_allocation_function_parameter const, void const * const pointer, size_t const size)
{
	if (!size)
	{
		void*	non_const_pointer = const_cast<LPVOID>(pointer);
		xr_free(non_const_pointer);
		return 0;
	}

	if (!pointer)
	{
		return	(Memory.mem_alloc(size));
	}

	void*		non_const_pointer = const_cast<void*>(pointer);
	return		(Memory.mem_realloc(non_const_pointer, size));
}

void setup_luabind_allocator()
{
    luabind::allocator = &luabind_allocator;
    luabind::allocator_parameter = 0;
}

CVMLua::CVMLua()
{
	m_virtual_machine = luaL_newstate();
	R_ASSERT2(m_virtual_machine, "Cannot initialize script virtual machine!");

	OpenLib();
}

CVMLua::~CVMLua()
{
	lua_close(m_virtual_machine);
}

void CVMLua::OpenLib()
{
	lopen::openlua(m_virtual_machine);
	// FX to ALL: Add anothres namespace into this function
	luabind::open(m_virtual_machine);
}

void CVMLua::Add(AddFun pFun)
{
	pFun(m_virtual_machine);
}