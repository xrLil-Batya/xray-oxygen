#pragma once

#include "WeaponMagazined.h"
#include "../xrScripts/export/script_export_space.h"

class CWeaponFORT: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
protected:
public:
					CWeaponFORT			();
	virtual			~CWeaponFORT		();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponFORT)
#undef script_type_list
#define script_type_list save_type_list(CWeaponFORT)
