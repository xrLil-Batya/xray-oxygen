////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_storage.h
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Evaluation functions storage class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xrScripts/export/script_export_space.h"

#define AI_MAX_EVALUATION_FUNCTION_COUNT	128

class CGameObject;
class CEntityAlive;
class CSE_ALifeSchedulable;
class CSE_ALifeObject;
class CBaseFunction;
class CPatternFunction;

class CDistanceFunction;
class CEquipmentType;
class CItemDeterioration;
class CEquipmentPreference;
class CMainWeaponType;
class CMainWeaponPreference;
class CItemValue;
class CWeaponAmmoCount;
class CDetectorType;
class CPersonalCreatureTypeFunction;
class CPersonalRelationFunction;
class CPersonalGreedFunction;
class CPersonalAggressivenessFunction;
class CEnemyEquipmentCostFunction;
class CEnemyRukzakWeightFunction;
class CEnemyAnomalityFunction;
class CPersonalWeaponTypeFunction;

template <typename T1, typename T2>
class CEF_Params
{
private:
	T1* m_member;
	T1* m_enemy;
	T2* m_member_item;
	T2* m_enemy_item;

public:
	IC	void	clear()
	{
		m_member = 0;
		m_enemy = 0;
		m_member_item = 0;
		m_enemy_item = 0;
	}

	IC CEF_Params       () { clear(); }
	IC	T1*& member     () { return (m_member); }
	IC	T1*& enemy      () { return (m_enemy); }
	IC	T2*& member_item() { return (m_member_item); }
	IC	T2*& enemy_item () { return (m_enemy_item); }
};

typedef CEF_Params<const CEntityAlive,const CGameObject>		CNonALifeParams;
typedef CEF_Params<CSE_ALifeSchedulable,const CSE_ALifeObject>	CALifeParams;

template <class T>
class CEnemyFunction;

class CEF_Storage 
{
	using CEnemyWeaponType = CEnemyFunction<CPersonalWeaponTypeFunction>;
public:
	CNonALifeParams							m_non_alife_params;
	CALifeParams							m_alife_params;
	// primary functions
	CBaseFunction							*m_fpaBaseFunctions		[AI_MAX_EVALUATION_FUNCTION_COUNT];

	CDistanceFunction						*m_pfDistance;
	CEquipmentType							*m_pfEquipmentType;
	CMainWeaponType							*m_pfMainWeaponType;
	CMainWeaponPreference					*m_pfMainWeaponPreference;
	CItemValue								*m_pfItemValue;
	CWeaponAmmoCount						*m_pfWeaponAmmoCount;
	CDetectorType							*m_pfDetectorType;
	
	CPersonalCreatureTypeFunction			*m_pfPersonalCreatureType;
	CPersonalWeaponTypeFunction				*m_pfPersonalWeaponType;
	CPersonalRelationFunction				*m_pfPersonalRelation;
	CPersonalGreedFunction					*m_pfPersonalGreed;
	CPersonalAggressivenessFunction			*m_pfPersonalAggressiveness;

	CEnemyEquipmentCostFunction				*m_pfEnemyEquipmentCost;
	CEnemyRukzakWeightFunction				*m_pfEnemyRukzakWeight;
	CEnemyAnomalityFunction					*m_pfEnemyAnomality;
	CEnemyWeaponType						*m_pfEnemyWeaponType;

	// complex functions
	CPatternFunction						*m_pfWeaponEffectiveness;
	CPatternFunction						*m_pfVictoryProbability;
	CPatternFunction						*m_pfExpediency;
	CPatternFunction						*m_pfMainWeaponValue;
	CPatternFunction						*m_pfSmallWeaponValue;
	CPatternFunction						*m_pfBirthPercentage;
	CPatternFunction						*m_pfBirthProbability;
	CPatternFunction						*m_pfBirthSpeed;
	CPatternFunction						*m_pfEntityCost;

											CEF_Storage		();
	virtual									~CEF_Storage	();
			CBaseFunction					*function		(LPCSTR function) const;
	IC		void							alife_evaluation(bool value);
	IC		CNonALifeParams					&non_alife		();
	IC		CALifeParams					&alife			();
};

#include "ef_storage_inline.h"