#pragma once

#include "WeaponMagazined.h"
#include "../xrScripts/export/script_export_space.h"

class CWeaponSVU :
	public CWeaponMagazined
{
	typedef CWeaponMagazined inherited;
public:
	CWeaponSVU(void);
	virtual ~CWeaponSVU(void);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponSVU)
#undef script_type_list
#define script_type_list save_type_list(CWeaponSVU)
