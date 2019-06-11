////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_low_cover_planner.h
//	Created 	: 04.09.2007
//  Modified 	: 04.09.2007
//	Author		: Dmitriy Iassenev
//	Description : Stalker low cover planner
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "action_planner_action_script.h"

class CAI_Stalker;

class stalker_low_cover_planner : public CActionPlannerActionScript<CAI_Stalker> 
{
	using inherited = CActionPlannerActionScript<CAI_Stalker>;

			void	add_evaluators				();
			void	add_actions					();

public:
					stalker_low_cover_planner	(CAI_Stalker *object = 0, LPCSTR action_name = "");
	virtual	void	setup						(CAI_Stalker *object, CPropertyStorage *storage);
	virtual void	update						();
	virtual void	initialize					();
};