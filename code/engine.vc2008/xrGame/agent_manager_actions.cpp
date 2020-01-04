////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_actions.cpp
//	Created 	: 04.01.2020
//	Author		: ForserX
//	Description : Template Agent mngr action for EAgentMngrType
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "agent_manager_actions.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "agent_location_manager.h"
#include "agent_corpse_manager.h"
#include "agent_explosive_manager.h"
#include "agent_enemy_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "sight_action.h"
#include "inventory.h"

CAgentManagerActionTemplate::CAgentManagerActionTemplate(CAgentManager *object, const char* action_name, EAgentMngrType eType) :
	inherited(object,action_name)
{
	eActionType = eType;
}

void CAgentManagerActionTemplate::finalize()
{
	inherited::finalize();

	if(eActionType == EAgentMngrType::eNoOrders)
		m_object->corpse().clear();
}

void CAgentManagerActionTemplate::initialize		()
{
	inherited::initialize();
	
	if(eActionType == EAgentMngrType::eKillEnemy || eActionType == EAgentMngrType::eReactOnDanger)
	m_object->location().clear();
}

void CAgentManagerActionTemplate::execute()
{
	inherited::execute();

	if(eActionType == EAgentMngrType::eKillEnemy)
		m_object->enemy().distribute_enemies();
	
	if(eActionType == EAgentMngrType::eKillEnemy || eActionType == EAgentMngrType::eReactOnDanger)
	{
		m_object->explosive().react_on_explosives();
		m_object->corpse().react_on_member_death();
	}
}