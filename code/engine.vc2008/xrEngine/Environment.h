#pragma once

// refs
class ENGINE_API IRender_Visual;
class ENGINE_API CInifile;
class ENGINE_API CEnvironment;

// refs - effects
class ENGINE_API CEnvironment;
class ENGINE_API CLensFlare;
class ENGINE_API CEffectRain;
class ENGINE_API CEffect_Thunderbolt;

class ENGINE_API CPerlinNoise1D;

class CThunderboltDesc;
class CThunderboltCollection;
class CLensFlareDescriptor;

#define DAY_LENGTH 86400.f

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/EnvironmentRender.h"

#include "EnvModifier.h"
#include "EnvAmbient.h"
#include "EnvDescriptor.h"
#include "EnvDescriptorMixer.h"

class ENGINE_API CEnvironment
{
	friend class dxEnvironmentRender;
	struct str_pred
	{	
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{	return xr_strcmp(x,y)<0;	}
	};
public:
	using EnvAmbVec = xr_vector<CEnvAmbient*>;
	using EnvVec = xr_vector<CEnvDescriptor*>;
	using EnvsMap = xr_map<shared_str, EnvVec, str_pred>;
private:
	// clouds
	FvectorVec				CloudsVerts;
	U16Vec					CloudsIndices;

	int						LoadCounter = 0;

public:
	float					NormalizeTime	(float tm);
private:
	float					TimeDiff		(float prev, float cur);
	float					TimeWeight		(float val, float min_t, float max_t);
	void					SelectEnvs		(EnvVec* envs, CEnvDescriptor*& e0, CEnvDescriptor*& e1, float tm);
	void					SelectEnv		(EnvVec* envs, CEnvDescriptor*& e, float tm);

public:
	static bool				lb_env_pred			(const CEnvDescriptor* x, float val)				{ return x->exec_time < val; }
	static bool				sort_env_pred		(const CEnvDescriptor* x, const CEnvDescriptor* y)	{ return x->exec_time < y->exec_time; }
	static bool				sort_env_etl_pred	(const CEnvDescriptor* x, const CEnvDescriptor* y)	{ return x->exec_time_loaded < y->exec_time_loaded; }

    void					CalculateDynamicSunDir();

protected:
	CPerlinNoise1D*			PerlinNoise1D;

	float					fGameTime;
public:
	FactoryPtr<IEnvironmentRender> m_pRender;
	bool					bNeed_re_create_env;

	float					wind_strength_factor;	
	float					wind_gust_factor;

	// wind blast params
	float					wind_blast_strength;
	Fvector					wind_blast_direction;
	Fquaternion				wind_blast_start_time;
	Fquaternion				wind_blast_stop_time;
	float					wind_blast_strength_start_value;
	float					wind_blast_strength_stop_value;
	Fquaternion				wind_blast_current;
	// Environments
	CEnvDescriptorMixer*	CurrentEnv;
	CEnvDescriptor*			Current[2];

	bool					bWFX;
	float					wfx_time;
	CEnvDescriptor*			WFX_end_desc[2];
    
    EnvVec*					CurrentWeather;
    shared_str				CurrentWeatherName;
	shared_str				CurrentCycleName;

	EnvsMap					WeatherCycles;
	EnvsMap					WeatherFXs;
	xr_vector<CEnvModifier>	Modifiers;
	EnvAmbVec				Ambients;

	CEffectRain*			eff_Rain;
	CLensFlare*				eff_LensFlare;
	CEffect_Thunderbolt*	eff_Thunderbolt;

	float					fTimeFactor;

    void					SelectEnvs			(float gt);

	void					UpdateAmbient		();
	INGAME_EDITOR_VIRTUAL CEnvAmbient* AppendEnvAmb	(const shared_str& sect);

	void					Invalidate			();
public:
							CEnvironment		();

	INGAME_EDITOR_VIRTUAL	~CEnvironment		();

	INGAME_EDITOR_VIRTUAL void	Load			();
    INGAME_EDITOR_VIRTUAL void	Unload			();

	void					ModsLoad			();
	void					ModsUnload			();

	void					OnFrame				();
	void					Lerp				(float& current_weight);

	void					RenderSky			();
	void					RenderClouds		();
	void					RenderFlares		();
	void					RenderLast			();

	bool					SetWeatherFX		(shared_str name);
	bool					StartWeatherFXFromTime	(shared_str name, float time);
	bool					IsWeatherFXPlaying	(){return bWFX;}
	void					StopWeatherFX		();

	void					SetWeather			(shared_str name, bool forced=false);
    shared_str				GetWeather			()					{ return CurrentWeatherName;}
	void					ChangeGameTime		(float game_time);
	void					SetGameTime			(float game_time, float time_factor);

	void					Statistics			(CGameFont* pFont);

	void					OnDeviceCreate		();
	void					OnDeviceDestroy		();

	// editor-related
#	ifdef INGAME_EDITOR
		float				GetGameTime			(){return fGameTime;}
#	endif // #ifdef INGAME_EDITOR

	bool					m_paused;

	CInifile*				m_ambients_config;
	CInifile*				m_sound_channels_config;
	CInifile*				m_effects_config;
	CInifile*				m_suns_config;
	CInifile*				m_thunderbolt_collections_config;
	CInifile*				m_thunderbolts_config;

protected:
	INGAME_EDITOR_VIRTUAL	CEnvDescriptor* CreateDescriptor	(shared_str const& identifier, CInifile* config);
	INGAME_EDITOR_VIRTUAL	void LoadWeathers					();
	INGAME_EDITOR_VIRTUAL	void LoadWeatherEffects				();
	INGAME_EDITOR_VIRTUAL	void CreateMixer					();
							void DestroyMixer					();

							void LoadLevelSpecificAmbients		();

public:
	INGAME_EDITOR_VIRTUAL	CThunderboltDesc* ThunderboltDescription		(CInifile& config, shared_str const& section);
	INGAME_EDITOR_VIRTUAL	CThunderboltCollection* ThunderboltCollection	(CInifile* pIni, CInifile* thunderbolts, LPCSTR section);
	INGAME_EDITOR_VIRTUAL	CThunderboltCollection* ThunderboltCollection	(xr_vector<CThunderboltCollection*>& collection,  shared_str const& id);
	INGAME_EDITOR_VIRTUAL	CLensFlareDescriptor*	AddFlare				(xr_vector<CLensFlareDescriptor*>& collection, shared_str const& id);

public:
	float						p_var_alt;
	float						p_var_long;
	float						p_min_dist;
	float						p_tilt;
	float						p_second_prop;
	float						p_sky_color;
	float						p_sun_color;
	float						p_fog_color;
};

ENGINE_API CEnvironment& Environment();
