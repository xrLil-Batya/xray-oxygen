////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat action classes
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "stalker_combat_action_base.h"
#include "random32.h"

class CCoverPoint;

namespace MonsterSpace 
{
	enum EBodyState;
	enum EMovementType;
}

//////////////////////////////////////////////////////////////////////////
class CStalkerActionGetItemToKill : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionGetItemToKill	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionMakeItemKilling : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionMakeItemKilling	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionRetreatFromEnemy : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionRetreatFromEnemy	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		execute							();
	virtual void		finalize						();
	virtual _edge_value_type	weight					(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionGetReadyToKill : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

private:
	MonsterSpace::EBodyState		m_body_state;
	MonsterSpace::EMovementType		m_movement_type;
	bool							m_affect_properties;
	bool							m_enable_enemy_change;

public:
						CStalkerActionGetReadyToKill(bool affect_properties, CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionKillEnemy : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionKillEnemy		(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionTakeCover : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

private:
	MonsterSpace::EBodyState		m_body_state;
	MonsterSpace::EMovementType		m_movement_type;

public:
						CStalkerActionTakeCover		(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionLookOut : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

private:
	u32					m_last_change_time;
	CRandom32			m_crouch_look_out_random;

public:
						CStalkerActionLookOut		(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionHoldPosition : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionHoldPosition	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionDetourEnemy : public CStalkerActionCombatBase 
{
	const CCoverPoint	*m_enemy_cover;

protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionDetourEnemy	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionPostCombatWait : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionPostCombatWait(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionHideFromGrenade : public CStalkerActionCombatBase
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionHideFromGrenade(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionSuddenAttack : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionSuddenAttack	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionKillEnemyIfPlayerOnThePath : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionKillEnemyIfPlayerOnThePath(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize								();
	virtual void		execute									();
	virtual void		finalize								();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerActionCriticalHit : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;

public:
						CStalkerActionCriticalHit	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerCombatActionThrowGrenade : public CStalkerActionCombatBase 
{
protected:
	using inherited = CStalkerActionCombatBase;
	ALife::_OBJECT_ID m_grenade_id;

public:
						CStalkerCombatActionThrowGrenade(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};

//////////////////////////////////////////////////////////////////////////
class CStalkerCombatActionSmartCover : public CStalkerActionCombatBase
{
	using inherited = CStalkerActionCombatBase;
	bool m_check_can_kill_enemy;

public:
						CStalkerCombatActionSmartCover	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};