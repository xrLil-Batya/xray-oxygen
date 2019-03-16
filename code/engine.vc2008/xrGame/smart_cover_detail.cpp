////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cover_detail.cpp
//	Created 	: 22.08.2007
//	Author		: Alexander Dudin
//	Description : detail namespace functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smart_cover_detail.h"
#include <luabind/luabind.hpp>

float smart_cover::detail::parse_float	(
		luabind::object const &table,
		LPCSTR identifier,
		float const &min_threshold,
		float const &max_threshold
	)
{
	VERIFY2			(table.type() == LUA_TTABLE, "invalid loophole description passed");
	luabind::object	lua_result = table[identifier];
	VERIFY_FORMAT (lua_result.type() != LUA_TNIL, "cannot read number value %s", identifier);
	VERIFY_FORMAT (lua_result.type() == LUA_TNUMBER, "cannot read number value %s", identifier);
	float			result = luabind::object_cast<float>(lua_result);
	VERIFY_FORMAT(result >= min_threshold, "invalid read number value %s", identifier);
	VERIFY_FORMAT(result <= max_threshold, "invalid number value %s", identifier);
	return			(result);
}

LPCSTR smart_cover::detail::parse_string(luabind::object const &table, LPCSTR identifier)
{
	VERIFY2			(table.type() == LUA_TTABLE, "invalid loophole description passed");
	luabind::object	result = table[identifier];
	VERIFY_FORMAT(result.type() != LUA_TNIL, "cannot read string value %s", identifier);
	VERIFY_FORMAT(result.type() == LUA_TSTRING, "cannot read string value %s", identifier);
	return			(luabind::object_cast<LPCSTR>(result));
}

void smart_cover::detail::parse_table	(luabind::object const &table, LPCSTR identifier, luabind::object &result)
{
	VERIFY2			(table.type() == LUA_TTABLE, "invalid loophole description passed");
	result			= table[identifier];
	VERIFY_FORMAT(result.type() != LUA_TNIL, "cannot read table value %s", identifier);
	VERIFY_FORMAT(result.type() == LUA_TTABLE, "cannot read table value %s", identifier);
}

bool smart_cover::detail::parse_bool	(luabind::object const &table, LPCSTR identifier)
{
	VERIFY2			(table.type() == LUA_TTABLE, "invalid loophole description passed");
	luabind::object	result = table[identifier];
	VERIFY_FORMAT(result.type() != LUA_TNIL, "cannot read boolean value %s", identifier);
	VERIFY_FORMAT(result.type() == LUA_TBOOLEAN, "cannot read boolean value %s", identifier);
	return			(luabind::object_cast<bool>(result));
}

int smart_cover::detail::parse_int		(luabind::object const &table, LPCSTR identifier)
{
	VERIFY2			(table.type() == LUA_TTABLE, "invalid loophole description passed");
	luabind::object	result = table[identifier];
	VERIFY_FORMAT(result.type() != LUA_TNIL, "cannot read number value %s", identifier);
	VERIFY_FORMAT(result.type() == LUA_TNUMBER, "cannot read number value %s", identifier);
	return			(luabind::object_cast<int>(result));
}

Fvector smart_cover::detail::parse_fvector (luabind::object const &table, LPCSTR identifier)
{
	VERIFY2			(table.type() == LUA_TTABLE, "invalid loophole description passed");
	luabind::object	result = table[identifier];
	VERIFY_FORMAT(result.type() != LUA_TNIL, "cannot read vector value %s", identifier);
	return			(luabind::object_cast<Fvector>(result));
}