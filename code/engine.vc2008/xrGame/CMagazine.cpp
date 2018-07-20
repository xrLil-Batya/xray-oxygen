#include "stdafx.h"
#include "CMagazine.h"


CMagazine::CMagazine()
{
}


CMagazine::~CMagazine()
{
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