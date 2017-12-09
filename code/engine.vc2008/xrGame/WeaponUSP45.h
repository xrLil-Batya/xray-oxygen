#pragma once

#include "WeaponMagazined.h"
#include "../xrScripts/export/script_export_space.h"

class CWeaponUSP45 :
	public CWeaponMagazined
{
	typedef CWeaponMagazined inherited;
public:
				CWeaponUSP45();
	virtual		~CWeaponUSP45();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponUSP45)
#undef script_type_list
#define script_type_list save_type_list(CWeaponUSP45)
