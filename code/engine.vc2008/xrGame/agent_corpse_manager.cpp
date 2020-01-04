////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_corpse_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 04.01.2020
//	Author		: Dmitriy Iassenev
//	Description : Agent corpse manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_corpse_manager.h"
#include "member_order.h"
#include "ai/stalker/ai_stalker.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "agent_manager.h"
#include "agent_member_manager.h"

bool CAgentCorpseManager::process_corpse	(CMemberOrder &member)
{
	float min_dist_sqr = flt_max;
	CMemberCorpse *best_corpse = nullptr;

	for (CMemberCorpse &ObjMember : m_corpses)
	{
		if (!member.object().memory().visual().visible_now(ObjMember.corpse()))
			continue;

		float dist_sqr = ObjMember.corpse()->Position().distance_to_sqr(member.object().Position());
		if (dist_sqr < min_dist_sqr) 
		{
			if(ObjMember.reactor() && (ObjMember.reactor()->Position().distance_to_sqr(ObjMember.corpse()->Position()) <= min_dist_sqr))
				continue;

			min_dist_sqr	= dist_sqr;
			best_corpse		= &ObjMember;
		}
	}
	
	if (!best_corpse)
		return				(false);

	best_corpse->reactor	(&member.object());
	return					(true);
}

void CAgentCorpseManager::react_on_member_death()
{
	while (true)
	{
		bool changed = false;
		for (CMemberOrder* pMember : object().member().combat_members())
		{
			if (!pMember->member_death_reaction().m_processing)
				changed = process_corpse(*pMember);
		}

		if (!changed)
			break;
	}

	for (CMemberCorpse &ObjectMember : m_corpses)
	{
		if (!ObjectMember.reactor())
			continue;

		CMemberOrder::CMemberDeathReaction	&reaction = object().member().member(ObjectMember.reactor()).member_death_reaction();
		reaction.m_member = ObjectMember.corpse();
		reaction.m_time = ObjectMember.time();
		reaction.m_processing = true;
	}

	m_corpses.erase
	(
		std::remove_if(m_corpses.begin(), m_corpses.end(), [](CMemberCorpse &corpse) { return !!corpse.reactor();}, m_corpses.end()
	);
}

void CAgentCorpseManager::remove_links	(CObject *object)
{
	m_corpses.erase(std::remove_if
	(
		m_corpses.begin(), m_corpses.end(), 
		[object](CMemberCorpse &corpse){return corpse.corpse()->ID() == object->ID();},
		m_corpses.end()
	));
}

void CAgentCorpseManager::update()
{
}
