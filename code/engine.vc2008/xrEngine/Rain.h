#pragma once

#include "../xrCDB/xr_collide_defs.h"
#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/RainRender.h"

class ENGINE_API CEffectRain
{
	friend class dxRainRender;
private:
	// Timers for simulating smooth wetting/drying effect
	struct RainTimer
	{
		bool			bFirstRainingFrame;
		float			rainTimestamp;
		float			rainDropTimeBasic;
		float			previousFrameTime;
		float			lastRainDuration;
		float			rainDropTime;
		float			rainTimer;

		RainTimer() : bFirstRainingFrame(true), rainTimestamp(0.0f), rainDropTimeBasic(20.0f), previousFrameTime(0.0f),
			lastRainDuration(0.0f), rainDropTime(0.0f), rainTimer(0.0f)
		{
		};
	};
	// Rain droplet (while flying)
	struct Item
	{
		Fvector			P;
		Fvector			Phit;
		Fvector			D;
		float			fSpeed;
		u32				dwTime_Life;
		u32				dwTime_Hit;
		u32				uv_set;
		u16				targetMaterial;
		void			Invalidate()
		{
			dwTime_Life	= 0;
		}
	};
	// Drop particles (when hitted something)
	struct Particle
	{
		Particle		*next,*prev;
		Fmatrix			mXForm;
		Fsphere			bounds;
		float			time;
	};
	enum States
	{
		stIdle		= 0,
		stWorking
	};
public:
	struct RainParams
	{
		float			rainTimer;
	};

private:
	// Visualization (rain and drops)
	FactoryPtr<IRainRender>			m_pRender;

	// Data and logic
	xr_vector<Item>					items;
	States							state;

	// Particles
	xr_vector<Particle>				particle_pool;
	Particle*						particle_active;
	Particle*						particle_idle;

	// Sounds
	float							sndVolume;

	ref_sound						snd_Ambient;

	RainTimer*						timerWorld;
	RainTimer*						timerCurrViewEntity;

private:
	// Utilities
	void							ParticlePoolCreate	();
	void							ParticlePoolDestroy	();

	void							ParticleRemove	(Particle* P, Particle* &LST);
	void							ParticleInsert	(Particle* P, Particle* &LST);
	int								ParticleListSize(Particle* LST);
	Particle*						ParticleAllocate();
	void							ParticleFree	(Particle* P);

	// Some methods
	void							Born			(Item& dest, float radius);
	void							Hit				(Item& item);
	bool							RayPick			(const Fvector& s, const Fvector& d, float& range, u16& targetMaterial, collide::rq_target tgt);
	void							RenewItem		(Item& dest, float height, u16 targetMaterial, bool bHit);
public:
									CEffectRain		();
									~CEffectRain	();

	float							GetWorldWetness			() const;
	float							GetCurrViewEntityWetness() const;

	void							Load			();
	void							Render			();
	void							UpdateTimer		(RainTimer& timer, States state, bool bNeedRayPick);
	void							OnFrame			();
	void							InvalidateState	();
};
