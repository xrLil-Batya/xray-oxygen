////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_by_sound_planner.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger by sound planner class
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerDangerBySoundPlanner : public CActionPlannerActionScript<CAI_Stalker> 
{
	using inherited = CActionPlannerActionScript<CAI_Stalker>;

protected:
			void		add_evaluators					();
			void		add_actions						();

public:
						CStalkerDangerBySoundPlanner	(CAI_Stalker *object = 0, LPCSTR action_name = "");
	virtual	void		setup							(CAI_Stalker *object, CPropertyStorage *storage);
};
