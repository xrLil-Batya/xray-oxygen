#pragma once
#include "../action_planner.h"
#include "../property_evaluator_const.h"

class CUIMapWnd;
class CMapActionPlanner :public CActionPlanner<CUIMapWnd,true>
{
private:
	using inherited = CActionPlanner<CUIMapWnd,true>;

public:
					CMapActionPlanner	();
	virtual			~CMapActionPlanner	();
	virtual	void	setup				(CUIMapWnd *object);
	virtual	LPCSTR	object_name			() const;
};

