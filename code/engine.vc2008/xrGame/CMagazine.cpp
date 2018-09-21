#include "stdafx.h"
#include "CMagazine.h"

CMagazine::CMagazine() : m_pCurrentAmmo(nullptr)
{
	m_iAmmoCurrentTotal = 0;

	iAmmoElapsed = -1;
	iMagazineSize = -1;
	m_ammoType = 0;
}

CMagazine::~CMagazine()
{
//	xr_delete(m_pCurrentAmmo);
}

float CMagazine::GetMagazineWeight(const decltype(CMagazine::m_magazine)& mag) const
{
	float res = 0;
	const char* last_type = nullptr;
	float last_ammo_weight = 0;
	for (auto& c : mag)
	{
		// Usually ammos in mag have same type, use this fact to improve performance
		if (last_type != c.m_ammoSect.c_str())
		{
			last_type = c.m_ammoSect.c_str();
			last_ammo_weight = c.Weight();
		}
		res += last_ammo_weight;
	}
	return res;
}