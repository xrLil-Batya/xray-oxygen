#include "stdafx.h"
#include "script_sound_info.h"
#include "script_game_object.h"

#include <luabind/luabind.hpp>
using namespace luabind;

#pragma optimize("gyts",on)
void CScriptSoundInfo::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptSoundInfo>("SoundInfo")
			.def_readwrite("who",				&CScriptSoundInfo::who)
			.def_readwrite("danger",			&CScriptSoundInfo::dangerous)
			.def_readwrite("position",			&CScriptSoundInfo::position)
			.def_readwrite("power",				&CScriptSoundInfo::power)
			.def_readwrite("time",				&CScriptSoundInfo::time)
	];
}
