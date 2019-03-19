// Author: Charsi82
// Modifer: ForserX

#include "stdafx.h"
#include "ActorRuck.h"

void CActorRuck::Load(LPCSTR section)
{
	inherited::Load(section);
	m_additional_weight = pSettings->r_float(section, "additional_inventory_weight");
}