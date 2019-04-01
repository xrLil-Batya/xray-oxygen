#pragma once

class ENGINE_API CEnvironment;
class ENGINE_API CEnvAmbient;

class ENGINE_API CEnvDescriptor
{
public:
	float				exec_time;
	float				exec_time_loaded;

	shared_str			sky_texture_name;
	shared_str			sky_texture_env_name;
	shared_str			clouds_texture_name;

	FactoryPtr<IEnvDescriptorRender> m_pDescriptor;

	Fvector4			clouds_color;
	Fvector3			sky_color;
	float				sky_rotation;

	float				far_plane;

	Fvector3			fog_color;
	float				fog_density;
	float				fog_distance;

	float				rain_density;
	Fvector3			rain_color;

	float				bolt_period;
	float				bolt_duration;

    float				wind_velocity;
    float				wind_direction;  
    
	Fvector3			ambient	;
	Fvector4			hemi_color;	// w = R2 correction
	Fvector3			sun_color;
	Fvector3			sun_dir;
	float				m_fSunShaftsIntensity;
	float				m_fWaterIntensity;
	float 				m_fTreeAmplitudeIntensity;

	shared_str			lens_flare_id;
	shared_str			tb_id;
    
	CEnvAmbient*		env_ambient;


						CEnvDescriptor	(shared_str const& identifier);

	void				Load			(CEnvironment& environment, CInifile& config);
	void				Copy			(const CEnvDescriptor& src);

	void				OnDeviceCreate	();
	void				OnDeviceDestroy	();

	shared_str			m_identifier;
};
