////////////////////////////////////////////////////////////////////////////
//	Module 		: script_particle_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script particle action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_particle_action.h"
#include "../xrParticles/psystem.h"
#include "../xrParticles/particlesobject.h"

CScriptParticleAction::~CScriptParticleAction()
{
}

void CScriptParticleAction::SetParticle			(LPCSTR caParticleToRun, bool bAutoRemove)
{
	m_caParticleToRun	= caParticleToRun;
	m_tGoalType			= eGoalTypeParticleAttached;
	m_tpParticleSystem	= CParticlesObject::Create(*m_caParticleToRun,BOOL(m_bAutoRemove = bAutoRemove));
	m_bStartedToPlay	= false;
	m_bCompleted		= false;
}
