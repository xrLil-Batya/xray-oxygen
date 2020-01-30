#pragma once

class CAI_Bloodsucker;
class CAlienEffector;
class CAlienEffectorPP;

class CBloodsuckerAlien
{
	CAI_Bloodsucker				*m_object;

	CAlienEffector				*m_effector;
	CAlienEffectorPP			*m_effector_pp;
	
	bool						m_crosshair_show;
	bool						m_active;

public:
			CBloodsuckerAlien	();
			~CBloodsuckerAlien	();
	
	void	InitExternal		(CAI_Bloodsucker *obj);	
	void	Reinit				();

	void	Switch				(bool bActive);
IC	bool	Active				() const {return m_active;}

};