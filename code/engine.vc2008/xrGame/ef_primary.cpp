////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_primary.cpp
//	Created 	: 13.06.2003
//  Modified 	: 13.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Primary evaluation function classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "items/Weapon.h"
#include "entity_alive.h"
#include "inventoryowner.h"
#include "alife_simulator.h"
#include "ef_storage.h"
#include "ai_space.h"
#include "inventory.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ef_primary.h"
#include "alife_human_brain.h"
#include "alife_human_object_handler.h"

IC	CLASS_ID CBaseFunction::clsid_member_item() const
{
	CLASS_ID result;
	if (ef_storage().non_alife().member_item())
		result = ef_storage().non_alife().member_item()->CLS_ID;
	else 
	{
		VERIFY2 (ef_storage().alife().member_item(),"No object specified for evaluation function");
		result = ef_storage().alife().member_item()->m_tClassID;
	}
	return (result);
}

IC	CLASS_ID CBaseFunction::clsid_enemy_item() const
{
	CLASS_ID result;
	if (ef_storage().non_alife().enemy_item())
		result = ef_storage().non_alife().enemy_item()->CLS_ID;
	else 
	{
		VERIFY2 (ef_storage().alife().enemy_item(),"No object specified for evaluation function");
		result = ef_storage().alife().enemy_item()->m_tClassID;
	}
	return (result);
}

IC	CLASS_ID CBaseFunction::clsid_member() const
{
	CLASS_ID result;
	if (ef_storage().non_alife().member())
		result = ef_storage().non_alife().member()->CLS_ID;
	else 
	{
		VERIFY2(ef_storage().alife().member(),"No object specified for evaluation function");
		const CSE_ALifeDynamicObject *l_tpALifeDynamicObject = smart_cast<const CSE_ALifeDynamicObject*>(ef_storage().alife().member());
		VERIFY2(l_tpALifeDynamicObject,"Invalid object passed to the evaluation function");
		result = l_tpALifeDynamicObject->m_tClassID;
	}
	return (result);
}

IC	CLASS_ID CBaseFunction::clsid_enemy() const
{
	CLASS_ID result;
	if (ef_storage().non_alife().enemy())
		result = ef_storage().non_alife().enemy()->CLS_ID;
	else
	{
		VERIFY2(ef_storage().alife().enemy(),"No object specified for evaluation function");
		const CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = smart_cast<const CSE_ALifeDynamicObject*>(ef_storage().alife().enemy());
		VERIFY2(l_tpALifeDynamicObject,"Invalid object passed to the evaluation function");
		result = l_tpALifeDynamicObject->m_tClassID;
	}
	return (result);
}

float CDistanceFunction::ffGetValue()
{
	return (ef_storage().non_alife().member())
		? (ef_storage().non_alife().member()->Position().distance_to(ef_storage().non_alife().enemy()->Position()))
		: (ef_storage().alife().member()->base()->Position().distance_to(ef_storage().alife().enemy()->base()->Position()));
}
float CPersonalCreatureTypeFunction::ffGetValue()
{
	u32 result;
	if (ef_storage().non_alife().member())
		result = ef_storage().non_alife().member()->ef_creature_type();
	else
	{
		VERIFY2(ef_storage().alife().member(), "No object specified for evaluation function");
		result = ef_storage().alife().member()->ef_creature_type();
	}

	VERIFY(float(result) < m_fMaxResultValue + 1);
	return (float(result));
}

u32 CPersonalWeaponTypeFunction::dwfGetWeaponType()
{
	u32 result;
	if (ef_storage().non_alife().member_item())
		result = ef_storage().non_alife().member_item()->ef_weapon_type();
	else {
		VERIFY2(ef_storage().alife().member_item(), "No object specified for evaluation function");
		result = ef_storage().alife().member_item()->ef_weapon_type();
	}
	return (result);
}

float CPersonalWeaponTypeFunction::ffGetTheBestWeapon() 
{
	u32 dwBestWeapon = 0;
	
	if (ef_storage().non_alife().member() && ef_storage().non_alife().member_item())
		return (float(dwfGetWeaponType()));

	if (ef_storage().non_alife().member()) {
		const CInventoryOwner *tpInventoryOwner = smart_cast<const CInventoryOwner*>(ef_storage().non_alife().member());
		if (tpInventoryOwner) 
		{
			for (u16 it = tpInventoryOwner->inventory().FirstSlot(); it <= tpInventoryOwner->inventory().LastSlot(); ++it)
			{
				PIItem iitem = tpInventoryOwner->inventory().ItemFromSlot(it);
				if (iitem) 
				{
					CWeapon *tpCustomWeapon = smart_cast<CWeapon*>(iitem);
					if (tpCustomWeapon && (tpCustomWeapon->GetSuitableAmmoTotal(true) > tpCustomWeapon->GetAmmoMagSize() / 10)) 
					{
						ef_storage().non_alife().member_item() = tpCustomWeapon;
						u32 dwCurrentBestWeapon = dwfGetWeaponType();
						if (dwCurrentBestWeapon > dwBestWeapon)
							dwBestWeapon = dwCurrentBestWeapon;
						ef_storage().non_alife().member_item() = nullptr;
					}
				}
			}
		}
	}
	else 
	{
		dwBestWeapon = dwfGetWeaponType();
	}
	return(float(dwBestWeapon));
}

float CPersonalWeaponTypeFunction::ffGetValue()
{
	float result;
	if (ef_storage().non_alife().member())
		if (ef_storage().non_alife().member()->natural_weapon())
			result = (float)ef_storage().non_alife().member()->ef_weapon_type();
		else
			result = ffGetTheBestWeapon();
	else 
	{
		VERIFY2(ef_storage().alife().member(), "No object specified for evaluation function");
		if (ef_storage().alife().member()->natural_weapon())
			result = (float)ef_storage().alife().member()->ef_weapon_type();
		else
			result = ffGetTheBestWeapon();
	}
	VERIFY(result < m_fMaxResultValue + 1.f);
	return		(result);
}


float CPersonalRelationFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement relation function")
	return(0);
}

float CPersonalGreedFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement greed function")
	return(0);
}

float CPersonalAggressivenessFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement aggressiveness function")
	return(0);
}

float CEnemyEquipmentCostFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement enemy equipment cost function")
	return					(0);
}

float CEnemyRukzakWeightFunction::ffGetValue()
{
	float m_fLastValue;
	if (ef_storage().non_alife().member())
	{
		const CInventoryOwner* tpInventoryOwner = smart_cast<const CInventoryOwner*>(ef_storage().non_alife().member());
		if (tpInventoryOwner)
			m_fLastValue = tpInventoryOwner->inventory().TotalWeight();
		else
			m_fLastValue = 0;
	}
	else m_fLastValue = 0;
	return					(m_fLastValue);
}

float CEnemyAnomalityFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement enemy anomality function")
	return(0);
}
float CEquipmentType::ffGetValue()
{
	u32 result;
	if (ef_storage().non_alife().member_item())
		result	= ef_storage().non_alife().member_item()->ef_equipment_type();
	else {
		VERIFY2	(ef_storage().alife().member_item(),"No object specified for evaluation function");
		result	= ef_storage().alife().member_item()->ef_equipment_type();
	}
	VERIFY		(float(result) < m_fMaxResultValue + 1.f);
	return		(float(result));
}

float CMainWeaponType::ffGetValue()
{
	u32 result;
	if (ef_storage().non_alife().member_item())
		result	= ef_storage().non_alife().member_item()->ef_main_weapon_type();
	else {
		VERIFY2	(ef_storage().alife().member_item(),"No object specified for evaluation function");
		result	= ef_storage().alife().member_item()->ef_main_weapon_type();
	}
	VERIFY		(float(result) < m_fMaxResultValue + 1.f);
	return		(float(result));
}

float CMainWeaponPreference::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
#pragma todo("Dima to Dima : Append MainWeaponPreference with non-ALife branch")
		return					(0);
	}
	else {
		CSE_ALifeHumanAbstract* l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ef_storage().alife().member());
		R_ASSERT2(l_tpALifeHumanAbstract, "Non-human object in EquipmentPreference evaluation function");
		return					(l_tpALifeHumanAbstract->brain().m_cpMainWeaponPreferences[ef_storage().m_pfMainWeaponType->dwfGetDiscreteValue(iFloor(ef_storage().m_pfMainWeaponType->ffGetMaxResultValue() + .5f))]);
	}
}

float CItemValue::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
#pragma todo("Dima to Dima : Append ItemValue with non-ALife branch")
		return					(0);
	}
	else {
		const CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = smart_cast<const CSE_ALifeInventoryItem*>(ef_storage().alife().member_item());
		R_ASSERT2				(l_tpALifeInventoryItem,"Non-item object specified for the ItemDeterioration evaluation function");
		return					(float(l_tpALifeInventoryItem->m_dwCost));
	}
}

float CWeaponAmmoCount::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
#pragma todo("Dima to Dima : Append WeaponAmmoCount with non-ALife branch")
		return					(0);
	}
	else {
		CSE_ALifeHumanAbstract* l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ef_storage().alife().member());
		R_ASSERT2(l_tpALifeHumanAbstract, "Non-human object in WeaponAmmoCount evaluation function");
		return					(l_tpALifeHumanAbstract->brain().objects().get_available_ammo_count(smart_cast<const CSE_ALifeItemWeapon*>(ef_storage().alife().member_item()), l_tpALifeHumanAbstract->alife().m_temp_item_vector));
	}
}

u32	 CWeaponAmmoCount::dwfGetDiscreteValue(u32 dwDiscretizationValue)
{
	float fTemp = ffGetValue();
	if (fTemp <= m_fMinResultValue)
		return(0);
	else
		if (fTemp >= m_fMaxResultValue)
			return(dwDiscretizationValue - 1);
		else {
			const CSE_ALifeItemWeapon* l_tpALifeItemWeapon = smart_cast<const CSE_ALifeItemWeapon*>(ef_storage().alife().member_item());
			if (l_tpALifeItemWeapon && l_tpALifeItemWeapon->m_caAmmoSections) {
				string32		S;
				_GetItem(l_tpALifeItemWeapon->m_caAmmoSections, 0, S);
				u32				l_dwBoxSize = pSettings->r_s32(S, "box_size");
				if (fTemp <= 3 * l_dwBoxSize)
					return(iFloor(1 * float(dwDiscretizationValue) / 10 + .5f));
				return(iFloor(2 * float(dwDiscretizationValue) / 10 + .5f));
			}
			else
				return(dwDiscretizationValue - 1);
		}
}

float CDetectorType::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
		if (!ef_storage().non_alife().member_item())
			return				(0);
	}
	else {
		if (!ef_storage().alife().member_item())
			return				(0);
	}

	u32			result;
	if (ef_storage().non_alife().member())
		if (ef_storage().non_alife().member()->natural_detector())
			result	= ef_storage().non_alife().member()->ef_detector_type();
		else
			result	= ef_storage().non_alife().member_item()->ef_detector_type();
	else {
		VERIFY2	(ef_storage().alife().member(),"No object specified for evaluation function");
		if (ef_storage().alife().member()->natural_detector())
			result	= ef_storage().alife().member()->ef_detector_type();
		else
			result	= ef_storage().alife().member_item()->ef_detector_type();
	}
	VERIFY		(float(result) < m_fMaxResultValue + 1.f);
	return		(float(result));
}