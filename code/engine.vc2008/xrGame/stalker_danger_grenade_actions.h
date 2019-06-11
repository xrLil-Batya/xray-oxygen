////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_grenade_actions.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger grenade actions classes
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "stalker_combat_actions.h"

class CStalkerActionDangerGrenadeTakeCover : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionDangerGrenadeTakeCover	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize									();
	virtual void		execute										();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDangerGrenadeWaitForExplosion : public CStalkerActionCombatBase
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionDangerGrenadeWaitForExplosion		(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize									();
	virtual void		execute										();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDangerGrenadeTakeCoverAfterExplosion : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

private:
	bool				m_direction_sight;

public:
						CStalkerActionDangerGrenadeTakeCoverAfterExplosion	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize									();
	virtual void		execute										();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDangerGrenadeLookAround : public CStalkerActionCombatBase
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionDangerGrenadeLookAround	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize									();
	virtual void		execute										();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDangerGrenadeSearch : public CStalkerActionCombatBase
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionDangerGrenadeSearch	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize								();
};
