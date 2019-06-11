////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_kill_wounded_actions.h
//	Created 	: 25.05.2006
//  Modified 	: 25.05.2006
//	Author		: Dmitriy Iassenev
//	Description : Stalker kill wounded action classes
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
class CStalkerActionReachWounded : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionReachWounded	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionAimWounded : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionAimWounded	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionPrepareWounded : public CStalkerActionCombatBase
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionPrepareWounded(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionKillWounded : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionKillWounded	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionPauseAfterKill : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionPauseAfterKill(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};
