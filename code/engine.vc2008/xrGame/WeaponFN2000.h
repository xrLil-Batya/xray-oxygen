#pragma once

#include "WeaponMagazined.h"
#include "../xrScripts/export/script_export_space.h"

class CWeaponFN2000: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
public:
					CWeaponFN2000	();
	virtual			~CWeaponFN2000	();
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponFN2000)
#undef script_type_list
#define script_type_list save_type_list(CWeaponFN2000)
