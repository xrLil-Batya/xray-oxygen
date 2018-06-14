#include "stdafx.h"
#include "EffectorBobbing.h"

#include "actor.h"
#include "actor_defs.h"

static const char* BOBBING_SECT = "bobbing_effector";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorBobbing::CEffectorBobbing() : CEffectorCam(eCEBobbing, 10000.f)
{
	fTime = 0;
	fReminderFactor = 0;
	is_limping = false;

	m_fAmplitudeRun = pSettings->r_float(BOBBING_SECT, "run_amplitude");
	m_fAmplitudeWalk = pSettings->r_float(BOBBING_SECT, "walk_amplitude");
	m_fAmplitudeLimp = pSettings->r_float(BOBBING_SECT, "limp_amplitude");

	m_fCrouchFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "crouch_factor", 0.75f);
	m_fSpeedReminder = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "speed_reminder", 5.f);

	m_fSpeedRun = pSettings->r_float(BOBBING_SECT, "run_speed");
	m_fSpeedWalk = pSettings->r_float(BOBBING_SECT, "walk_speed");
	m_fSpeedLimp = pSettings->r_float(BOBBING_SECT, "limp_speed");
}

CEffectorBobbing::~CEffectorBobbing	() {}

void CEffectorBobbing::SetState(u32 mstate, bool limping, bool ZoomMode) 
{
	dwMState = mstate;
	is_limping = limping;
	m_bZoomMode = ZoomMode;
}

BOOL CEffectorBobbing::ProcessCam(SCamEffectorInfo& info)
{
	fTime += Device.fTimeDelta;
	if (dwMState&ACTOR_DEFS::mcAnyMove) 
	{
		if (fReminderFactor < 1.f)	
			fReminderFactor += m_fSpeedReminder * Device.fTimeDelta;
		else						
			fReminderFactor = 1.f;
	}
	else 
	{
		if (fReminderFactor > 0.f)	
			fReminderFactor -= m_fSpeedReminder * Device.fTimeDelta;
		else						
			fReminderFactor = 0.f;
	}

	if (!fsimilar(fReminderFactor, 0)) 
	{
		Fmatrix	M;
		M.identity();
		M.j.set(info.n);
		M.k.set(info.d);
		M.i.crossproduct(info.n, info.d);
		M.c.set(info.p);

		// apply footstep bobbing effect
		Fvector dangle;
		float k = ((dwMState& ACTOR_DEFS::mcCrouch) ? m_fCrouchFactor : 1.f);

		float A, ST;

		if (isActorAccelerated(dwMState, m_bZoomMode))
		{
			A = m_fAmplitudeRun * k;
			ST = m_fSpeedRun * fTime*k;
		}
		else if (is_limping)
		{
			A = m_fAmplitudeLimp * k;
			ST = m_fSpeedLimp * fTime*k;
		}
		else
		{
			A = m_fAmplitudeWalk * k;
			ST = m_fSpeedWalk * fTime*k;
		}

		float _sinA = _abs(_sin(ST)*A)*fReminderFactor;
		float _cosA = _cos(ST)*A*fReminderFactor;

		info.p.y += _sinA;
		dangle.x = _cosA;
		dangle.z = _cosA;
		dangle.y = _sinA;

		Fmatrix R;
		R.setHPB(dangle.x, dangle.y, dangle.z);

		Fmatrix mR;
		mR.mul(M, R);

		info.d.set(mR.k);
		info.n.set(mR.j);
	}

	return TRUE;
}
