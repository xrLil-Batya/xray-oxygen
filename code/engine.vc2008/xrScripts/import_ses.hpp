#pragma once
#include "xrScripts.h"
#include <Windows.h>

namespace import_ses
{
		void	LUACORE LuaLog		(const char* caMessage);
	ICF LPCSTR	LUACORE user_name	() { return (Core.UserName); };
}
