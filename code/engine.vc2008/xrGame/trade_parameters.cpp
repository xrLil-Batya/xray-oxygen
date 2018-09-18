////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_parameters.cpp
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade parameters class
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "trade_parameters.h"

CTradeParameters *CTradeParameters::m_instance = nullptr;

void CTradeParameters::process	(action_show, CInifile &ini_file, const shared_str &section)
{
	VERIFY					(ini_file.section_exist(section));
	m_show.clear			();
	for (auto& it: ini_file.r_section(section).Data)
	{
		if (!it.second.size())
			m_show.disable(it.first);
	}
}
