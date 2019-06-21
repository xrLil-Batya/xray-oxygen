#include "stdafx.h"
#include "ZoneCampfire.h"
#include "../xrParticles/psystem.h"
#include "../xrParticles/ParticlesObject.h"
#include "GamePersistent.h"
#include "../xrEngine/LightAnimLibrary.h"
#include "../xrEngine/Rain.h"

CZoneCampfire::CZoneCampfire() : m_pDisabledParticles(nullptr), m_pEnablingParticles(nullptr), m_turned_on(true), m_turn_time(0) {}

CZoneCampfire::~CZoneCampfire()
{
	CParticlesObject::Destroy(m_pDisabledParticles);
	CParticlesObject::Destroy(m_pEnablingParticles);
	m_sound.destroy();
}

void CZoneCampfire::Load(LPCSTR section)
{
	inherited::Load(section);
}

void CZoneCampfire::GoEnabledState()
{
	inherited::GoEnabledState();

	if (m_pDisabledParticles)
	{
		m_pDisabledParticles->Stop(false);
		CParticlesObject::Destroy(m_pDisabledParticles);
	}

	m_sound.stop();
	m_sound.destroy();

	LPCSTR str = pSettings->r_string(cNameSect(), "enabling_particles");
	m_pEnablingParticles = CParticlesObject::Create(str, false);
	m_pEnablingParticles->UpdateParent(XFORM(), zero_vel);
	m_pEnablingParticles->Play(false);

	str = pSettings->r_string(cNameSect(), "enabling_sound");
	m_sound.create(str, st_Effect, sg_SourceType);
	m_sound.play_at_pos(this, Position(), true);
}

void CZoneCampfire::GoDisabledState()
{
	inherited::GoDisabledState();

	if (!m_pDisabledParticles)
	{
		LPCSTR str = pSettings->r_string(cNameSect(), "disabled_particles");
		m_pDisabledParticles = CParticlesObject::Create(str, false);
		m_pDisabledParticles->UpdateParent(XFORM(), zero_vel);
		m_pDisabledParticles->Play(false);

		m_sound.stop();
		m_sound.destroy();

		const char* SndStr = pSettings->r_string(cNameSect(), "disabled_sound");
		m_sound.create(SndStr, st_Effect, sg_SourceType);
		m_sound.play_at_pos(this, Position(), true);
	}
}

static const int OVL_TIME = 3000;

void CZoneCampfire::turn_on_script()
{
	m_turn_time = Device.dwTimeGlobal + OVL_TIME;
	m_turned_on = true;
	GoEnabledState();
}

void CZoneCampfire::turn_off_script()
{
	m_turn_time = Device.dwTimeGlobal + OVL_TIME;
	m_turned_on = false;
	GoDisabledState();
}

void CZoneCampfire::shedule_Update(u32	dt)
{
	if (!IsEnabled() && m_turn_time)
		UpdateWorkload(dt);

	if (m_pIdleParticles)
	{
		Fvector vel;
		vel.mul(Environment().wind_blast_direction, Environment().wind_strength_factor);
		m_pIdleParticles->UpdateParent(XFORM(), vel);
	}

	inherited::shedule_Update(dt);
}

void CZoneCampfire::PlayIdleParticles(bool bIdleLight)
{
	if (m_turn_time == 0 || m_turn_time - Device.dwTimeGlobal < (OVL_TIME - 2000))
	{
		inherited::PlayIdleParticles(bIdleLight);
		if (m_pEnablingParticles)
		{
			m_pEnablingParticles->Stop(false);
			CParticlesObject::Destroy(m_pEnablingParticles);
		}
	}
}

void CZoneCampfire::StopIdleParticles(bool bIdleLight)
{
	if (m_turn_time == 0 || m_turn_time - Device.dwTimeGlobal < (OVL_TIME - 500))
		inherited::StopIdleParticles(bIdleLight);
}

BOOL CZoneCampfire::AlwaysTheCrow()
{
	return m_turn_time ? TRUE : inherited::AlwaysTheCrow();
}

void CZoneCampfire::UpdateWorkload(u32 dt)
{
	inherited::UpdateWorkload(dt);

	if (m_turned_on && Environment().eff_Rain->GetWorldWetness() > 0.4)
		turn_off_script();

	if (m_turn_time > Device.dwTimeGlobal)
	{
		float k = float(m_turn_time - Device.dwTimeGlobal) / float(OVL_TIME);

		if (m_turned_on)
		{
			k = 1.0f - k;
			PlayIdleParticles(true);
			StartIdleLight();
		}
		else
			StopIdleParticles(false);

		if (m_pIdleLight && m_pIdleLight->get_active())
		{
			VERIFY(m_pIdleLAnim);
			int frame = 0;
			u32 clr = m_pIdleLAnim->CalculateBGR(Device.fTimeGlobal, frame);
			Fcolor fclr;
			fclr.set(((float)color_get_B(clr) / 255.f) * k, ((float)color_get_G(clr) / 255.f) * k, ((float)color_get_R(clr) / 255.f) * k, 1.f);

			float range = m_fIdleLightRange + 0.25f * ::Random.randF(-1.f, 1.f);
			range *= k;

			m_pIdleLight->set_range(range);
			m_pIdleLight->set_color(fclr);
		}
	}
	else if (m_turn_time)
	{
		m_turn_time = 0;
		m_turned_on ? PlayIdleParticles(true) : StopIdleParticles(true);
	}
}