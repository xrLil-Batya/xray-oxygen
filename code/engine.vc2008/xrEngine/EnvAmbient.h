#pragma once

class ENGINE_API CEnvAmbient
{
public:
	struct SEffect
	{
		u32 			life_time;
		ref_sound		sound;		
		shared_str		particles;
		Fvector			offset;
		float			wind_gust_factor;
		float			wind_blast_in_time;
		float			wind_blast_out_time;
		float			wind_blast_strength;
		Fvector			wind_blast_direction;

		INGAME_EDITOR_VIRTUAL	~SEffect				()	{}
	};
	using EffectVec = xr_vector<SEffect*>;
	struct ENGINE_API SSndChannel
	{
		shared_str				m_load_section;
		Fvector2				m_sound_dist;
		Ivector4				m_sound_period;

		typedef xr_vector<ref_sound>	sounds_type;

		void					Load					(CInifile& config, LPCSTR sect);
		ref_sound&				get_rnd_sound			()	{return sounds()[::Random.randI(s32(sounds().size()))];}
		u32						get_rnd_sound_time		()	{return (m_sound_period.z < m_sound_period.w) ? ::Random.randI(m_sound_period.z,m_sound_period.w) : 0;}
		u32						get_rnd_sound_first_time()	{return (m_sound_period.x < m_sound_period.y) ? ::Random.randI(m_sound_period.x,m_sound_period.y) : 0;}
		float					get_rnd_sound_dist		()	{return (m_sound_dist.x < m_sound_dist.y) ? ::Random.randF(m_sound_dist.x, m_sound_dist.y) : 0;}
		INGAME_EDITOR_VIRTUAL	~SSndChannel			()	{}
		inline INGAME_EDITOR_VIRTUAL sounds_type& sounds()  {return m_sounds;}

	protected:
		xr_vector<ref_sound>	m_sounds;
	};
	using SSndChannelVec = xr_vector<SSndChannel*>;
protected:
	shared_str				m_load_section;

	EffectVec				m_effects;
	Ivector2				m_effect_period;

	SSndChannelVec			m_sound_channels;
	shared_str              m_ambients_config_filename;

public:
	IC const shared_str&	name				()	{return m_load_section;}
	IC const shared_str&	get_ambients_config_filename ()	{return m_ambients_config_filename;}

	INGAME_EDITOR_VIRTUAL	void Load(CInifile& ambients_config, CInifile& sound_channels_config, CInifile& effects_config, const shared_str& section);
	IC SEffect*				get_rnd_effect		()	{return effects().empty()?0:effects()[::Random.randI(s32(effects().size()))];}
	IC u32					get_rnd_effect_time ()	{return ::Random.randI(m_effect_period.x, m_effect_period.y);}

	INGAME_EDITOR_VIRTUAL	SEffect*		CreateEffect			(CInifile& config, LPCSTR id);
	INGAME_EDITOR_VIRTUAL	SSndChannel*	CreateSoundChannel	(CInifile& config, LPCSTR id);
	INGAME_EDITOR_VIRTUAL					~CEnvAmbient			();
							void			Destroy					();
	inline INGAME_EDITOR_VIRTUAL EffectVec&			effects			() { return m_effects; }
	inline INGAME_EDITOR_VIRTUAL SSndChannelVec&	get_snd_channels() { return m_sound_channels; }
};
