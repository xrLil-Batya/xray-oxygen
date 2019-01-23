#include "stdafx.h"
#pragma hdrstop

#include "EnvDescriptorMixer.h"
#include "EnvModifier.h"
#include "../Include/xrRender/EnvironmentRender.h"
#include "../xrServerEntities/LevelGameDef.h"

ENGINE_API extern float		psVisDistance;
ENGINE_API extern float		psFogDistance;
ENGINE_API extern float		psFogDensity;
ENGINE_API extern float		psFogNear;
ENGINE_API extern float		psFogFar;
ENGINE_API extern Fvector3	psFogColor;

ENGINE_API float ps_r_sunshafts_intensity = 0.0f;

//-----------------------------------------------------------------------------
// Environment Mixer
//-----------------------------------------------------------------------------
CEnvDescriptorMixer::CEnvDescriptorMixer(shared_str const& identifier) : CEnvDescriptor(identifier)
{
}

void CEnvDescriptorMixer::Destroy()
{
	m_pDescriptorMixer->Destroy();
	OnDeviceDestroy();
}

void CEnvDescriptorMixer::Clear()
{
	m_pDescriptorMixer->Clear();
}

void CEnvDescriptorMixer::Lerp(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& Mdf, float modifier_power)
{
	float modif_power	= 1.0f / (modifier_power + 1);	// the environment itself
	float fi			= 1.0f - f;

	m_pDescriptorMixer->Lerp(&*A.m_pDescriptor, &*B.m_pDescriptor);

	weight				= f;
	clouds_color.lerp	(A.clouds_color, B.clouds_color, f);
	sky_rotation		= (fi*A.sky_rotation + f * B.sky_rotation);

	// Far plane
	if (Mdf.use_flags.test(eViewDist))
		far_plane = (fi*A.far_plane + f * B.far_plane + Mdf.far_plane)*psVisDistance*modif_power;
	else
		far_plane = (fi*A.far_plane + f * B.far_plane)*psVisDistance;
	
	// Fog
	{
		bool bOverrideFogMixer = strstr(Core.Params, "-fog_mixer");

		if (bOverrideFogMixer)
		{
			// Color
			fog_color.set	(psFogColor);

			// Density
			fog_density		= (fi*A.fog_density + f * B.fog_density)*psFogDensity;
			if (Mdf.use_flags.test(eFogDensity))
			{
				fog_density += Mdf.fog_density;
				fog_density *= modif_power;
			}

			// Distance
			fog_distance	= (fi*A.fog_distance + f * B.fog_distance)*psFogDistance;
			// Near plane
			fog_near		= (1.0f - fog_density)*0.85f * fog_distance*psFogNear;
			// Far plane
			fog_far			= 0.99f * fog_distance*psFogFar;
		}
		else
		{
			// Color
			fog_color.lerp	(A.fog_color, B.fog_color, f);
			if (Mdf.use_flags.test(eFogColor))
				fog_color.add(Mdf.fog_color).mul(modif_power);

			// Density
			fog_density		= (fi*A.fog_density + f * B.fog_density);
			if (Mdf.use_flags.test(eFogDensity))
			{
				fog_density += Mdf.fog_density;
				fog_density *= modif_power;
			}

			// Distance
			fog_distance	= (fi*A.fog_distance + f * B.fog_distance);
			// Near plane
			fog_near		= (1.0f - fog_density)*0.85f * fog_distance;
			// Far plane
			fog_far			= 0.99f * fog_distance;
		}
	}

	// Rain
	{
		rain_density	= fi * A.rain_density + f * B.rain_density;
		rain_color.lerp	(A.rain_color, B.rain_color, f);
	}

	// Thunderbolt
	{
		bolt_period		= fi * A.bolt_period + f * B.bolt_period;
		bolt_duration	= fi * A.bolt_duration + f * B.bolt_duration;
	}

	// Wind
	{
		wind_velocity	= fi * A.wind_velocity + f * B.wind_velocity;
		wind_direction	= fi * A.wind_direction + f * B.wind_direction;
	}

	// Various factors
	{
		if (ps_r_sunshafts_intensity > 0.f)
			m_fSunShaftsIntensity = ps_r_sunshafts_intensity;
		else
			m_fSunShaftsIntensity = fi * A.m_fSunShaftsIntensity + f * B.m_fSunShaftsIntensity;

		m_fWaterIntensity			= fi * A.m_fWaterIntensity + f * B.m_fWaterIntensity;
		m_fTreeAmplitudeIntensity	= fi * A.m_fTreeAmplitudeIntensity + f * B.m_fTreeAmplitudeIntensity;
	}

	// Colors
	{
		// Sky
		sky_color.lerp(A.sky_color, B.sky_color, f);
		if (Mdf.use_flags.test(eSkyColor))
			sky_color.add(Mdf.sky_color).mul(modif_power);

		// Ambient
		ambient.lerp(A.ambient, B.ambient, f);
		if (Mdf.use_flags.test(eAmbientColor))
			ambient.add(Mdf.ambient).mul(modif_power);

		// Hemi
		hemi_color.lerp(A.hemi_color, B.hemi_color, f);
		if (Mdf.use_flags.test(eHemiColor))
		{
			hemi_color.x += Mdf.hemi_color.x;
			hemi_color.y += Mdf.hemi_color.y;
			hemi_color.z += Mdf.hemi_color.z;
			hemi_color.x *= modif_power;
			hemi_color.y *= modif_power;
			hemi_color.z *= modif_power;
		}

		// Sun
		sun_color.lerp(A.sun_color, B.sun_color, f);
	}

	R_ASSERT(_valid(A.sun_dir));
	R_ASSERT(_valid(B.sun_dir));
	sun_dir.lerp(A.sun_dir, B.sun_dir, f).normalize();
	R_ASSERT(_valid(sun_dir));

	VERIFY2(sun_dir.y < 0, "Invalid sun direction settings while lerp");
}
