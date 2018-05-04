////////////////////////////////////////////////////////////////////////////
//	Module 		: particle_params.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Particle parameters class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "particle_params.h"

CParticleParams::CParticleParams(const Fvector &tPositionOffset, const Fvector &tAnglesOffset, const Fvector &tVelocity)
{
	m_tParticlePosition = tPositionOffset;
	m_tParticleAngles = tAnglesOffset;
	m_tParticleVelocity = tVelocity;
}
#pragma warning(disable: 4459)
#include <luabind\luabind.hpp>

using namespace luabind;
#pragma optimize("s",on)
void CParticleParams::script_register(lua_State *L)
{
	module(L)
		[
			class_<CParticleParams>("particle_params")
			.def(constructor<>())
		.def(constructor<const Fvector &>())
		.def(constructor<const Fvector &, const Fvector &>())
		.def(constructor<const Fvector &, const Fvector &, const Fvector &>())
		];
}