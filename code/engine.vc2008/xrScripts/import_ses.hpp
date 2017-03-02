#pragma once
#include "xrScripts.h"
#include <Windows.h>

namespace import_ses
{
	void	LUACORE LuaLog(const char* caMessage);
	LPCSTR	LUACORE user_name();
}
