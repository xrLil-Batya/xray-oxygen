#include "stdafx.h"
#pragma hdrstop

#include "EnvAmbient.h"
#include "../xrServerEntities/object_broker.h"

//-----------------------------------------------------------------------------
// Environment ambient
//-----------------------------------------------------------------------------
void CEnvAmbient::SSndChannel::Load(CInifile& config, LPCSTR sect)
{
	m_load_section		= sect;

	m_sound_dist.x		= config.r_float(m_load_section, "min_distance");
	m_sound_dist.y		= config.r_float(m_load_section, "max_distance");
	m_sound_period.x	= config.r_s32	(m_load_section, "period0");
	m_sound_period.y	= config.r_s32	(m_load_section, "period1");
	m_sound_period.z	= config.r_s32	(m_load_section, "period2");
	m_sound_period.w	= config.r_s32	(m_load_section, "period3");

	R_ASSERT				(m_sound_period.x <= m_sound_period.y && m_sound_period.z <= m_sound_period.w);
	R_ASSERT2				(m_sound_dist.y > m_sound_dist.x, sect);

	LPCSTR snds				= config.r_string(sect,"sounds");
	u32 cnt					= _GetItemCount(snds);
	string_path				tmp;
	R_ASSERT3				(cnt, "sounds empty", sect);

	m_sounds.resize			(cnt);

	for (u32 i = 0; i < cnt; ++i)
	{
		_GetItem			(snds, i, tmp);
		m_sounds[i].create	(tmp, st_Effect, sg_SourceType);
	}
}

CEnvAmbient::SEffect* CEnvAmbient::CreateEffect(CInifile& config, LPCSTR id)
{
	SEffect* result				= xr_new<SEffect>();
	result->life_time			= iFloor(config.r_float(id, "life_time")*1000.f);
	result->particles			= config.r_string(id, "particles");		
	VERIFY						(result->particles.size());
	result->offset				= config.r_fvector3(id, "offset");
	result->wind_gust_factor	= config.r_float(id, "wind_gust_factor");
	
	if (config.line_exist(id, "sound"))
		result->sound.create(config.r_string(id, "sound"), st_Effect, sg_SourceType);

	if (config.line_exist(id, "wind_blast_strength"))
	{
		result->wind_blast_strength			= config.r_float(id, "wind_blast_strength");
		result->wind_blast_direction.setHP	(deg2rad(config.r_float(id, "wind_blast_longitude")), 0.f);
		result->wind_blast_in_time			= config.r_float(id, "wind_blast_in_time");
		result->wind_blast_out_time			= config.r_float(id, "wind_blast_out_time");
		return result;
	}

	result->wind_blast_strength			= 0.0f;
	result->wind_blast_direction.set	(0.0f, 0.0f, 1.0f);
	result->wind_blast_in_time			= 0.0f;
	result->wind_blast_out_time			= 0.0f;

	return result;
}

CEnvAmbient::SSndChannel* CEnvAmbient::CreateSoundChannel(CInifile& config, LPCSTR id)
{
	SSndChannel* result = xr_new<SSndChannel>();
	result->Load(config, id);

	return result;
}

CEnvAmbient::~CEnvAmbient()
{
	Destroy();
}

void CEnvAmbient::Destroy()
{
	delete_data(m_effects);
	delete_data(m_sound_channels);
}

void CEnvAmbient::Load(CInifile& ambients_config, CInifile& sound_channels_config, CInifile& effects_config, const shared_str& sect)
{
	m_ambients_config_filename = ambients_config.fname();
	m_load_section = sect;
	string_path tmp;

	// Sounds
	LPCSTR channels = ambients_config.r_string(sect, "sound_channels");
	u32 cnt = _GetItemCount(channels);
	m_sound_channels.resize(cnt);

	for (u32 i = 0; i < cnt; ++i)
		m_sound_channels[i] = CreateSoundChannel(sound_channels_config, _GetItem(channels, i, tmp));

	// Effects
	m_effect_period.set(iFloor(ambients_config.r_float(sect, "min_effect_period")*1000.f), iFloor(ambients_config.r_float(sect, "max_effect_period")*1000.f));
	LPCSTR effs = ambients_config.r_string(sect, "effects");
	cnt = _GetItemCount(effs);

	m_effects.resize(cnt);
	for (u32 i = 0; i < cnt; ++i)
		m_effects[i] = CreateEffect(effects_config, _GetItem(effs, i, tmp));

	R_ASSERT(!m_sound_channels.empty() || !m_effects.empty());
}
