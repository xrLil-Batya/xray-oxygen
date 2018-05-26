// Singleton LuaVM 
// ForserX 27.05.2018 for xrOxygen

#include <lua.hpp>
#include "../xrCore/xrCore.h"
#include "VMLua.h"
#include "luaopen.hpp"

CVMLua* SCRIPT_API LVM = new CVMLua();

CVMLua::CVMLua()
{
	m_virtual_machine = luaL_newstate();
	R_ASSERT2(m_virtual_machine, "Cannot initialize script virtual machine!");
	this->OpenLib();
}

CVMLua::~CVMLua()
{
	lua_close(m_virtual_machine);
}

void CVMLua::OpenLib()
{
	lopen::openlua(m_virtual_machine);
	// FX to ALL: Add anothres namespace into this function
}