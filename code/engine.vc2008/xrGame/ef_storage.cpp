////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_storage.cpp
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Evaluation functions storage class
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ef_storage.h"
#include "ef_primary.h"
#include "ef_pattern.h"

CEF_Storage::CEF_Storage()
{	
    std::memset(m_fpaBaseFunctions, 0, sizeof(m_fpaBaseFunctions));
	
	m_fpaBaseFunctions[0]				= m_pfDistance				= new CDistanceFunction		(this);
	m_fpaBaseFunctions[2]				= m_pfEquipmentType			= new CEquipmentType		(this);
	m_fpaBaseFunctions[5]				= m_pfMainWeaponType		= new CMainWeaponType		(this);
	m_fpaBaseFunctions[6]				= m_pfMainWeaponPreference	= new CMainWeaponPreference (this);
	m_fpaBaseFunctions[7]				= m_pfItemValue				= new CItemValue			(this);
	m_fpaBaseFunctions[8]				= m_pfWeaponAmmoCount		= new CWeaponAmmoCount		(this);
	m_fpaBaseFunctions[9]				= m_pfDetectorType			= new CDetectorType			(this);	
	m_fpaBaseFunctions[23]				= m_pfPersonalCreatureType	= new CPersonalCreatureTypeFunction(this);	
	m_fpaBaseFunctions[24]				= m_pfPersonalWeaponType    = new CPersonalWeaponTypeFunction(this);
	m_fpaBaseFunctions[27]				= m_pfPersonalRelation		= new CPersonalRelationFunction(this);		
	m_fpaBaseFunctions[28]				= m_pfPersonalGreed			= new CPersonalGreedFunction(this);			
	m_fpaBaseFunctions[29]				= m_pfPersonalAggressiveness= new CPersonalAggressivenessFunction(this);
	m_fpaBaseFunctions[43]				= m_pfEnemyWeaponType		= new CEnemyWeaponType(this);		
	m_fpaBaseFunctions[44]				= m_pfEnemyEquipmentCost	= new CEnemyEquipmentCostFunction(this);		
	m_fpaBaseFunctions[45]				= m_pfEnemyRukzakWeight		= new CEnemyRukzakWeightFunction(this);		
	m_fpaBaseFunctions[46]				= m_pfEnemyAnomality		= new CEnemyAnomalityFunction(this);

	m_pfWeaponEffectiveness				= new CPatternFunction("WeaponEffectiveness.efd",		this);
	m_pfExpediency						= new CPatternFunction("Expediency.efd",				this);
	m_pfMainWeaponValue					= new CPatternFunction("MainWeaponValue.efd",			this);
	m_pfSmallWeaponValue				= new CPatternFunction("SmallWeaponValue.efd",			this);
	m_pfBirthPercentage					= new CPatternFunction("BirthPercentage.efd",			this);
	m_pfBirthProbability				= new CPatternFunction("BirthProbability.efd",			this);
	m_pfBirthSpeed						= new CPatternFunction("BirthSpeed.efd",				this);
	m_pfVictoryProbability				= new CPatternFunction("WeaponEffectiveness.efd",		this);

	// Using into m_pfMainWeaponValue
	m_pfEntityCost = new CPatternFunction("EntityCost.efd", this);
}

CEF_Storage::~CEF_Storage()
{
	for (int i = 0; i < AI_MAX_EVALUATION_FUNCTION_COUNT; ++i)
		xr_delete(m_fpaBaseFunctions[i]);
}

CBaseFunction* CEF_Storage::function(LPCSTR function) const
{
	for (int i = 0; i < AI_MAX_EVALUATION_FUNCTION_COUNT; ++i) 
	{
		if (!xr_strcmp(function, m_fpaBaseFunctions[i]->Name()))
			return (m_fpaBaseFunctions[i]);
	}

	return nullptr;
}
