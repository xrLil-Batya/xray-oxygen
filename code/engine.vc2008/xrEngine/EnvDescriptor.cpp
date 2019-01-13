#include "stdafx.h"
#pragma hdrstop

#include "EnvDescriptor.h"

#include "Environment.h"
#include "../Include/xrRender/EnvironmentRender.h"
#include "LensFlare.h"
#include "Thunderbolt.h"

//-----------------------------------------------------------------------------
// Environment descriptor
//-----------------------------------------------------------------------------
CEnvDescriptor::CEnvDescriptor	(shared_str const& identifier) :
	m_identifier		(identifier)
{
	exec_time			= 0.0f;
	exec_time_loaded	= 0.0f;
	
	clouds_color.set	(1,1,1,1);
	sky_color.set		(1,1,1);
	sky_rotation		= 0.0f;

	far_plane			= 400.0f;;

	fog_color.set		(1,1,1);
	fog_density			= 0.0f;
	fog_distance		= 400.0f;

	rain_density		= 0.0f;
	rain_color.set		(0,0,0);

	bolt_period			= 0.0f;
	bolt_duration		= 0.0f;

    wind_velocity		= 0.0f;
    wind_direction		= 0.0f;
    
	ambient.set			(0,0,0);
	hemi_color.set		(1,1,1,1);
	sun_color.set		(1,1,1);
	sun_dir.set			(0,-1,0);

	m_fSunShaftsIntensity		= 1.0f;
	m_fWaterIntensity			= 1.0f;
	m_fTreeAmplitudeIntensity	= 0.01f;
	
    lens_flare_id		= "";
	tb_id				= "";
    
	env_ambient			= nullptr;
}

void CEnvDescriptor::Copy(const CEnvDescriptor& src)
{
	float tm0			= exec_time;
	float tm1			= exec_time_loaded;
	*this				= src;
	exec_time			= tm0;
	exec_time_loaded	= tm1;
}

#define	C_CHECK(C)	if (C.x<0 || C.x>2 || C.y<0 || C.y>2 || C.z<0 || C.z>2)	{ Msg("! Invalid '%s' in env-section '%s'", #C, identifier);}
void CEnvDescriptor::Load(CEnvironment& environment, CInifile& config)
{
	LPCSTR identifier		= m_identifier.c_str();

	Ivector3 tm				= { 0, 0, 0 };
	sscanf					(identifier, "%d:%d:%d", &tm.x, &tm.y, &tm.z);
	R_ASSERT3				((tm.x >= 0) && (tm.x < 24) && (tm.y >= 0) && (tm.y < 60) && (tm.z >= 0) && (tm.z < 60), "Incorrect weather time", identifier);
	exec_time				= tm.x*3600.0f + tm.y*60.0f + tm.z;
	exec_time_loaded		= exec_time;

	string_path st, st_env;
	xr_strcpy				(st, config.r_string(identifier, "sky_texture"));
	xr_strconcat			(st_env, st, "#small");
	sky_texture_name		= st;
	sky_texture_env_name	= st_env;
	clouds_texture_name		= config.r_string(identifier,"clouds_texture");
	LPCSTR cldclr			= config.r_string(identifier,"clouds_color");
	float multiplier = 0;
	float save = 0;
	sscanf					(cldclr, "%f,%f,%f,%f,%f", &clouds_color.x, &clouds_color.y, &clouds_color.z, &clouds_color.w, &multiplier);
	save					= clouds_color.w;
	clouds_color.mul		(0.5f*multiplier);
	clouds_color.w			= save; 
	
	sky_color				= config.r_fvector3	(identifier,"sky_color");
	
	if (config.line_exist(identifier,"sky_rotation"))
		sky_rotation		= deg2rad(config.r_float(identifier,"sky_rotation"));
	else
		sky_rotation	= 0;

	far_plane				= config.r_float	(identifier,"far_plane");
	fog_color				= config.r_fvector3	(identifier,"fog_color");
	fog_density				= config.r_float	(identifier,"fog_density");
	fog_distance			= config.r_float	(identifier,"fog_distance");
	rain_density			= config.r_float	(identifier,"rain_density"); clamp(rain_density,0.0f,1.0f);
	rain_color				= config.r_fvector3	(identifier,"rain_color");            
	wind_velocity			= config.r_float	(identifier,"wind_velocity");
	wind_direction			= deg2rad(config.r_float(identifier,"wind_direction"));
	ambient					= config.r_fvector3	(identifier,"ambient_color");
	hemi_color				= config.r_fvector4	(identifier,"hemisphere_color");
	sun_color				= config.r_fvector3	(identifier,"sun_color");

	float sun_altitude		= deg2rad(config.r_float(identifier, "sun_altitude"));
	float sun_longtitide	= deg2rad(config.r_float(identifier, "sun_longitude"));
	sun_dir.setHP			(sun_altitude, sun_longtitide);
	R_ASSERT				(_valid(sun_dir));
	VERIFY2					(sun_dir.y < 0, "Invalid sun direction settings while loading");

	lens_flare_id	= environment.eff_LensFlare->AppendDef(environment, environment.m_suns_config, config.r_string(m_identifier, "sun"));
	tb_id			= environment.eff_Thunderbolt->AppendDef(environment, environment.m_thunderbolt_collections_config, environment.m_thunderbolts_config, config.r_string(m_identifier,"thunderbolt_collection"));
	bolt_period		= (tb_id.size()) ? config.r_float(m_identifier, "thunderbolt_period") : 0.0f;
	bolt_duration	= (tb_id.size()) ? config.r_float(m_identifier, "thunderbolt_duration") : 0.0f;
	env_ambient		= config.line_exist(m_identifier, "ambient") ? environment.AppendEnvAmb(config.r_string(m_identifier, "ambient")) : 0;
	
    if (config.line_exist(identifier, "sun_shafts_intensity"))
        m_fSunShaftsIntensity = config.r_float(identifier, "sun_shafts_intensity");

	if (config.line_exist(m_identifier,"water_intensity"))
		m_fWaterIntensity = config.r_float(m_identifier,"water_intensity");
	
	if (config.line_exist(m_identifier, "tree_amplitude_intensity"))
		m_fTreeAmplitudeIntensity = config.r_float(m_identifier, "tree_amplitude_intensity");

	C_CHECK(clouds_color);
	C_CHECK(sky_color);
	C_CHECK(fog_color);
	C_CHECK(rain_color);
	C_CHECK(ambient);
	C_CHECK(hemi_color);
	C_CHECK(sun_color);

	OnDeviceCreate();
}

void CEnvDescriptor::OnDeviceCreate()
{
	m_pDescriptor->OnDeviceCreate(*this);
}

void CEnvDescriptor::OnDeviceDestroy()
{
	m_pDescriptor->OnDeviceDestroy();
}