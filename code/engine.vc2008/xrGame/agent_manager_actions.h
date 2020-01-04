////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_actions.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager actions
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "action_base.h"

class CAgentManager;
using CAgentManagerActionBase = CActionBase<CAgentManager>;

enum EAgentMngrType : u8
{
	eNoOrders = 0,
	eGatherItems,
	eKillEnemy,
	eReactOnDanger
};

class CAgentManagerActionTemplate : public CAgentManagerActionBase
{
	EAgentMngrType eActionType;

protected:
	using inherited = CAgentManagerActionBase;

public:
	CAgentManagerActionTemplate(CAgentManager *object, LPCSTR action_name = "", EAgentMngrType eType = EAgentMngrType::eNoOrders);
};