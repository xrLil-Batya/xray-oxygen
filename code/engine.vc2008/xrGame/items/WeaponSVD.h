#pragma once

#include "items/WeaponMagazined.h"
#include "../xrScripts/export/script_export_space.h"

class CWeaponSVD :
	public CWeaponMagazined
{
	typedef CWeaponMagazined inherited;
protected:
	virtual void switch2_Fire	();
	virtual void OnAnimationEnd (u32 state);
public:
	CWeaponSVD(void);
	virtual ~CWeaponSVD(void);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponSVD)
#undef script_type_list
#define script_type_list save_type_list(CWeaponSVD)
