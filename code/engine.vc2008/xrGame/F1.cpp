#include "stdafx.h"
#include "f1.h"
#include "WeaponAmmo.h"
#include "FoodItem.h"
#include "ExplosiveItem.h"
#include "InventoryBox.h"

CF1::CF1(void) {
}

CF1::~CF1(void) {
}

using namespace luabind;

#pragma optimize("s",on)
void CF1::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CF1,CGameObject>("CF1")
			.def(constructor<>()),

		class_<CWeaponAmmo,CGameObject>("CWeaponAmmo")
			.def(constructor<>()),

		class_<CFoodItem,CGameObject>("CFoodItem")
			.def(constructor<>()),

		class_<CInventoryBox,CGameObject>("CInventoryBox")
			.def(constructor<>()),

		class_<CExplosiveItem,CGameObject>("CExplosiveItem")
			.def(constructor<>())
	];
}
