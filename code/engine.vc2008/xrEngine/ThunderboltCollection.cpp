#include "stdafx.h"
#pragma hdrstop

#include "ThunderboltCollection.h"
#include "IGame_Persistent.h"

CThunderboltCollection::CThunderboltCollection()
{
}

CThunderboltCollection::~CThunderboltCollection()
{
	for (CThunderboltDesc* pTBD : palette)
		xr_delete(pTBD);

	palette.clear();
}

void CThunderboltCollection::Load(CInifile* pIni, CInifile* thunderbolts, LPCSTR sect)
{
	section			= sect;
	int tb_count	= pIni->line_count(sect);
	for (int i = 0; i < tb_count; ++i)
	{
		LPCSTR N, V;
		if (pIni->r_line(sect, i, &N, &V))
			palette.push_back(Environment().ThunderboltDescription(*thunderbolts, N));
	}
}

CThunderboltDesc* CThunderboltCollection::GetRandomDesc()
{ 
	VERIFY(palette.size() > 0);

	return palette[Random.randI(u32(palette.size()))];
}
