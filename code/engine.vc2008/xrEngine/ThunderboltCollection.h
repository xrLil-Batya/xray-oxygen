#pragma once
#include "ThunderboltDesc.h"

class ENGINE_API CThunderboltCollection
{
public:
	using DescVec = xr_vector<CThunderboltDesc*>;
	DescVec				palette;
	shared_str			section;
public:
						CThunderboltCollection	();
						~CThunderboltCollection	();
	void				Load					(CInifile* pIni, CInifile* thunderbolts, LPCSTR sect);
	CThunderboltDesc*	GetRandomDesc			();
};
