////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_planner.cpp
//	Created 	: 24.05.2004
//  Modified 	: 02.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_manager.h"
#include "agent_manager_space.h"
#include "agent_manager_actions.h"
#include "agent_manager_properties.h"
#include "agent_manager_planner.h"

using namespace AgentManager;

void CAgentManagerPlanner::setup	(CAgentManager *object)
{
	inherited::setup		(object);
	
	clear					();
	add_evaluators			();
	add_actions				();
	
	CWorldState				goal;
	goal.clear				();
	goal.add_condition		(CWorldProperty(ePropertyOrders,true));
	set_target_state		(goal);
}

void CAgentManagerPlanner::add_evaluators	()
{
	add_evaluator(ePropertyOrders, new CAgentManagerPropertyEvaluatorConst(false,"property_order"));
	add_evaluator(ePropertyItem, new CAgentManagerPropertyTemplate(&object(), "property_item", EAgentEvaluatorMngr::eItem));
	add_evaluator(ePropertyEnemy, new CAgentManagerPropertyTemplate(&object(),"property_enemy", EAgentEvaluatorMngr::eEnemy));
	add_evaluator(ePropertyDanger, new CAgentManagerPropertyTemplate(&object(),"property_danger", EAgentEvaluatorMngr::eDanger));
}

void CAgentManagerPlanner::add_actions		()
{
	CAgentManagerActionBase	*action;

	action					= new CAgentManagerTemplate(&object(), "no_orders", EAgentMngrType::eNoOrders);
	add_condition			(action,ePropertyOrders,			false);
	add_condition			(action,ePropertyItem,				false);
	add_condition			(action,ePropertyDanger,			false);
	add_condition			(action,ePropertyEnemy,				false);
	add_effect				(action,ePropertyOrders,			true);
	add_operator			(eOperatorNoOrders,					action);

	action					= new CAgentManagerTemplate(&object(),"gather_items", EAgentMngrType::eGatherItems);
	add_condition			(action,ePropertyItem,				true);
	add_condition			(action,ePropertyEnemy,				false);
	add_condition			(action,ePropertyDanger,			false);
	add_effect				(action,ePropertyItem,				false);
	add_operator			(eOperatorGatherItem,				action);

	action					= new CAgentManagerTemplate(&object(),"kill_enemy", EAgentMngrType::eKillEnemy);
	add_condition			(action,ePropertyEnemy,				true);
	add_effect				(action,ePropertyEnemy,				false);
	add_operator			(eOperatorKillEnemy,				action);

	action					= new CAgentManagerTemplate(&object(),"react_on_danger", EAgentMngrType::eReactOnDanger);
	add_condition			(action,ePropertyEnemy,				false);
	add_condition			(action,ePropertyDanger,			true);
	add_effect				(action,ePropertyDanger,			false);
	add_operator			(eOperatorReactOnDanger,			action);
}

void CAgentManagerPlanner::remove_links		(CObject *object)
{
}
