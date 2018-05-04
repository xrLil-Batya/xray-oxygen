////////////////////////////////////////////////////////////////////////////
//	Module 		: particle_params.h
//	Created 	: 30.09.2003
//  Modified 	: 31.10.2017
//	Author		: Dmitriy Iassenev
//	Modifer		: ForserX
//	Description : Particle parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "psystem.h"
#include "../xrScripts/export/script_export_space.h"

class PARTICLES_API CParticleParams
{
public:
	Fvector m_tParticlePosition;
	Fvector m_tParticleAngles;
	Fvector m_tParticleVelocity;

public:
	CParticleParams(const Fvector &tPositionOffset = Fvector().set(0, 0, 0), const Fvector &tAnglesOffset = Fvector().set(0, 0, 0),
		const Fvector &tVelocity = Fvector().set(0, 0, 0));

	virtual			~CParticleParams() = default;
	void			initialize() {}

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CParticleParams)
#undef script_type_list
#define script_type_list save_type_list(CParticleParams)
