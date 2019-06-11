////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_by_sound_actions.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger by sound actions classes
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDangerBySoundListenTo : public CStalkerActionCombatBase 
{
	using inherited = CStalkerActionCombatBase;
public:
						CStalkerActionDangerBySoundListenTo	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize							();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDangerBySoundCheck : public CStalkerActionCombatBase
{
	using inherited = CStalkerActionCombatBase;
public:
						CStalkerActionDangerBySoundCheck	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize							();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDangerBySoundTakeCover : public CStalkerActionCombatBase
{
	using inherited = CStalkerActionCombatBase;
public:
						CStalkerActionDangerBySoundTakeCover(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize							();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDangerBySoundLookOut : public CStalkerActionCombatBase 
{
	using inherited = CStalkerActionCombatBase;
public:
						CStalkerActionDangerBySoundLookOut	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize							();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDangerBySoundLookAround : public CStalkerActionCombatBase 
{
	using inherited = CStalkerActionCombatBase;
public:
						CStalkerActionDangerBySoundLookAround	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize								();
};
