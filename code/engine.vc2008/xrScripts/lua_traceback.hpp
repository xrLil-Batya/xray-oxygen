////////////////////////////////////////////////////////////////////////////
//	Module 		: lua_tools.h
//	Created 	: 29.07.2014
//  Modified 	: 12.07.2017
//	Author		: Alexander Petrov
//	Modifer		: ForserX
//	Description : Lua functionality extension
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "xrScripts.h"
extern "C" {
#include <lua.h>
#include <luajit.h>
#include <lcoco.h>
};

class SCRIPT_API CLua_Traceback
{
public:
	CLua_Traceback(lua_State *L, int depth) { get_traceback(L, depth) ;};
	
private:
	void get_traceback(lua_State *L, int depth);
	 
public:
	const char* m_traceback;
}