#include "stdafx.h"
#include "weaponvintorez.h"
#include "WeaponMagazinedWGrenade.h"

CWeaponVintorez::CWeaponVintorez() : CWeaponMagazined(SOUND_TYPE_WEAPON_SNIPERRIFLE)
{}

CWeaponVintorez::~CWeaponVintorez()
{}


#include "luabind/luabind.hpp"
using namespace luabind;
#pragma optimize("s",on)
void CWeaponVintorez::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponVintorez,CGameObject>("CWeaponVintorez")
			.def(constructor<>()),

		class_<CWeaponMagazined, CGameObject>("CWeaponMagazined")
			.def(constructor<>()),

		class_<CWeaponMagazinedWGrenade, CGameObject>("CWeaponMagazinedWGrenade")
			.def(constructor<>())
	];
}
