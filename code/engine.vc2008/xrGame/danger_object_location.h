////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_object_location.h
//	Created 	: 24.05.2004
//  Modified 	: 17.02.2020
//	Author		: Dmitriy Iassenev
//  Modifer		: ForserX 
//	Description : Danger object location
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "danger_location.h"

class CGameObject;

class CDangerObjectLocation : public CDangerLocation
{
	const CGameObject *m_object;

public:
	IC CDangerObjectLocation	(const CGameObject *pObj, u32 LevelTime, u32 Interval, float Radius, const squad_mask_type &mask = squad_mask_type(-1))
	{
		m_object		= pObj;
		m_level_time	= LevelTime;
		m_interval		= Interval;
		m_radius		= Radius;
		m_mask.assign	(mask);
	}
	
	virtual const Fvector	&position   () const { return m_object->Position(); }
	virtual bool			useful	    () const { return (true); }
	virtual	bool			operator==  (const CObject *object) const { return (m_object->ID() == object->ID()); }
};