////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_properties.cpp
//	Created 	: 04.01.2020
//	Author		: ForserX
//	Description : Template for agent manager properties
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "agent_manager_properties.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "agent_manager_space.h"
#include "ai/stalker/ai_stalker.h"
#include "memory_manager.h"
#include "item_manager.h"
#include "enemy_manager.h"
#include "danger_manager.h"

CAgentManagerPropertyTemplate::CAgentManagerPropertyTemplate(CAgentManager *object, const char* evaluator_name, EAgentEvaluatorMngr eType) :
	inherited(object, evaluator_name)
{
	R_ASSERT3(eType != EAgentEvaluatorMngr::eBroken, "Broken evaluator type: %s!", evaluator_name)
	ManagerType = eType;
}

CAgentManagerPropertyTemplate::_value_type CAgentManagerPropertyTemplate::evaluate	()
{
	if (ManagerType == EAgentEvaluatorMngr::eEnemy)
	{
		for (CAgentMemberManager* pObject : m_object->member().combat_members())
		{
			VERIFY(pObject);
			if (pObject->object().memory().enemy().selected())
				return true;
		}
	}
	else
	{
		for (CAgentMemberManager* pObject : m_object->member().members())
		{
			VERIFY(pObject);

			if (ManagerType == EAgentEvaluatorMngr::eItem && pObject->object().memory().item().selected())
				return true;

			if (ManagerType == EAgentEvaluatorMngr::eDanger && pObject->object().memory().danger().selected())
				return true;
		}
	}
	return false;
}