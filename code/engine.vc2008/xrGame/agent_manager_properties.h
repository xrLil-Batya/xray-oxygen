////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_properties.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager properties
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "property_evaluator_const.h"
#include "property_evaluator_member.h"

class CAgentManager;
using CAgentManagerPropertyEvaluator		= CPropertyEvaluator<CAgentManager>;
using CAgentManagerPropertyEvaluatorConst 	= CPropertyEvaluatorConst<CAgentManager>;
using CAgentManagerPropertyEvaluatorMember 	= CPropertyEvaluatorMember<CAgentManager>;

enum EAgentEvaluatorMngr : u8
{
	eItem = 0,
	eEnemy,
	eDanger, 

	eBroken = u8(-1)
};

class CAgentManagerPropertyTemplate : public CAgentManagerPropertyEvaluator 
{
	EAgentEvaluatorMngr ManagerType;

protected:
	using inherited = CAgentManagerPropertyEvaluator;

public:
	CAgentManagerPropertyTemplate(CAgentManager* pObj = nullptr, const char* name = "", EAgentEvaluatorMngr eType = EAgentEvaluatorMngr::eBroken);
	virtual _value_type	evaluate();
};