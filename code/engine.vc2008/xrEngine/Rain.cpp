#include "stdafx.h"
#pragma once

#include "Rain.h"
#include "IGame_Persistent.h"
#include "Environment.h"
#include "GameMtlLib.h"

#ifdef _EDITOR
    #include "ui_toolscustom.h"
#else
    #include "Render.h"
	#include "IGame_Level.h"
	#include "../xrCDB/xr_area.h"
	#include "xr_object.h"
#endif

ENGINE_API int		max_desired_items	= 2500;
ENGINE_API float	source_radius		= 12.5f;				// Radius of rain source - how far from actor drops will spawn
ENGINE_API float	source_offset		= 40.0f;				// Height of rain source - from which height rain drops
ENGINE_API float	max_distance		= source_offset*1.25f;
ENGINE_API float	sink_offset			= -(max_distance-source_offset);
ENGINE_API float	drop_length			= 5.0f;					// Visual length of rain drop
ENGINE_API float	drop_width			= 0.30f;				// Visual width of rain drop
ENGINE_API float	drop_angle_rand		= 4.0f;					// Randomization angle for each drop
ENGINE_API float	drop_max_angle		= deg2rad(89.0f);		// Drop angle when wind velocity is maximal
ENGINE_API float	drop_max_wind_vel	= 20.0f;				// Max wind velocity after which there will be no difference for rain
ENGINE_API float	drop_speed_min		= 40.0f;
ENGINE_API float	drop_speed_max		= 85.0f;

ENGINE_API int		max_particles		= 1000;
ENGINE_API int		particles_cache		= 400;
ENGINE_API float	particles_time		= 0.3f;
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectRain::CEffectRain() : sndVolume(0.0f)
{
	state = stIdle;
	
	if (Sound)
		snd_Ambient.create("ambient\\rain", st_Effect, sg_Undefined);
	else
		Msg("! Rain sound ambient not created - sound engine is null");

	//#TODO: RainbowZerg to self: make separate values for wetting/drying speed and load them from config
	timerWorld			= xr_new<RainTimer>();
	timerCurrViewEntity = xr_new<RainTimer>();

	// Moved to p_Render constructor
	ParticlePoolCreate();
}

CEffectRain::~CEffectRain()
{
	if (::Sound != nullptr)
		snd_Ambient.destroy();

	xr_delete(timerWorld);
	xr_delete(timerCurrViewEntity);

	// Cleanup
	ParticlePoolDestroy();
}

void CEffectRain::Load()
{
	//#TODO: RainbowZerg to self: load all rain parameters from config, for each rain descriptor separately
}

float CEffectRain::GetWorldWetness() const
{
	float rainTimer			= timerWorld->rainTimer;
	float lastRainDuration	= timerWorld->lastRainDuration;
	float rainDropTime		= timerWorld->rainDropTime;

	float res = ((rainTimer - lastRainDuration) / rainDropTime + lerp(0.0f, saturate(lastRainDuration / rainDropTime), saturate(lastRainDuration)));
	return res > 0 ? saturate(res) : 0.f;
}

float CEffectRain::GetCurrViewEntityWetness() const
{
	float rainTimer			= timerCurrViewEntity->rainTimer;
	float lastRainDuration	= timerCurrViewEntity->lastRainDuration;
	float rainDropTime		= timerCurrViewEntity->rainDropTime;

	float res = ((rainTimer - lastRainDuration) / rainDropTime + lerp(0.0f, saturate(lastRainDuration / rainDropTime), saturate(lastRainDuration)));
	return res > 0 ? saturate(res) : 0.f;
}

// Born
void CEffectRain::Born(Item& dest, float radius)
{
	Fvector axis; axis.set(0.0f, -1.0f, 0.0f);
	// Randomized wind strength factor to simulate inconstant gusts of wind
	float gust			= Environment().wind_strength_factor;
	// Drop angle deviation coefficient
	float k				= Environment().CurrentEnv->wind_velocity * gust / drop_max_wind_vel;
	clamp				(k, 0.0f, 1.0f);
	float pitch			= drop_max_angle * k - PI_DIV_2;
    axis.setHP			(Environment().CurrentEnv->wind_direction, pitch);
    
	Fvector& view		= Device.vCameraPosition;
	float angle			= ::Random.randF(0, PI_MUL_2);
	float dist			= _sqrt(::Random.randF()) * radius;
	float x				= dist * _cos(angle);
	float z				= dist * _sin(angle);
	dest.D.random_dir	(axis, deg2rad(drop_angle_rand));
	dest.P.set			(x + view.x - dest.D.x * source_offset, source_offset + view.y, z + view.z - dest.D.z * source_offset);
//	dest.P.set			(x+view.x,height+view.y,z+view.z);
	dest.fSpeed			= ::Random.randF(drop_speed_min, drop_speed_max);

	float height		= max_distance;
	u16 targetMaterial  = GAMEMTL_NONE_IDX;

	// Check if droplet will hit something when fall down
	bool bHit			= RayPick(dest.P, dest.D, height, targetMaterial, collide::rqtBoth);
	RenewItem			(dest, height, targetMaterial, bHit);
}

bool CEffectRain::RayPick(const Fvector& s, const Fvector& d, float& range, u16& targetMaterial, collide::rq_target tgt)
{
	collide::rq_result RQ;
	CObject* E = g_pGameLevel->CurrentViewEntity();
	const bool bRes = g_pGameLevel->ObjectSpace.RayPick(s, d, range, tgt, RQ, E);
	if (bRes)
	{
		range = RQ.range;
		CDB::TRI const& triangle = *(g_pGameLevel->ObjectSpace.GetStaticTris() + RQ.element);
		targetMaterial = triangle.material;
	}
	
    return bRes;
}

void CEffectRain::RenewItem(Item& dest, float height, u16 targetMaterial, bool bHit)
{
	dest.uv_set = Random.randI(2);
    if (bHit)
	{
		dest.dwTime_Life	= Device.dwTimeGlobal + iFloor(1000.f*height / dest.fSpeed) - Device.dwTimeDelta;
		dest.dwTime_Hit		= Device.dwTimeGlobal + iFloor(1000.f*height / dest.fSpeed) - Device.dwTimeDelta;
		dest.Phit.mad(dest.P, dest.D, height);
		dest.targetMaterial = targetMaterial;
	}
	else
	{
		dest.dwTime_Life	= Device.dwTimeGlobal + iFloor(1000.f*height / dest.fSpeed) - Device.dwTimeDelta;
		dest.dwTime_Hit		= Device.dwTimeGlobal + iFloor(2 * 1000.f*height / dest.fSpeed) - Device.dwTimeDelta;
		dest.Phit.set(dest.P);
	}
}

void CEffectRain::OnFrame()
{
	if (!g_pGameLevel)
		return;

	// Parse states
	float factor				= Environment().CurrentEnv->rain_density;
	static float hemi_factor	= 0.0f;
	CObject* pCurrViewEntity 	= g_pGameLevel->CurrentViewEntity();
	if (pCurrViewEntity && pCurrViewEntity->renderable_ROS())
	{
		// Determine if we indoor/outdoor by hemi values
		float* hemi_cube		= pCurrViewEntity->renderable_ROS()->get_luminocity_hemi_cube();
		float hemi_val			= std::max(hemi_cube[0], hemi_cube[1]);
		hemi_val				= std::max(hemi_val,	 hemi_cube[2]);
		hemi_val				= std::max(hemi_val,	 hemi_cube[3]);
		hemi_val				= std::max(hemi_val,	 hemi_cube[5]);
		
		float f					= hemi_val;
		float t					= Device.fTimeDelta;
		clamp					(t, 0.001f, 1.0f);
		hemi_factor				= lerp(hemi_factor, f, t);
	}

	// Update world wetness timer
	UpdateTimer(*timerWorld, state, false);

	if (::Sound != nullptr)
	{
		switch (state)
		{
		case stIdle:
		{
			// Update current view entity wetness timer
			// Just drying up, no need for using raypick
			UpdateTimer(*timerCurrViewEntity, state, false);

			if (factor >= EPS_L)
			{
				state = stWorking;
				snd_Ambient.play(nullptr, sm_Looped);
				snd_Ambient.set_position(Fvector().set(0, 0, 0));
				snd_Ambient.set_range(source_offset, source_offset*2.0f);
			}
		}
		break;
		case stWorking:
		{
			if (factor < EPS_L)
			{
				state = stIdle;
				snd_Ambient.stop();
				return;
			}
			// Update current view entity wetness timer
			// Use raypick to determine whether actor is under cover or not
			UpdateTimer(*timerCurrViewEntity, state, true);
		}
		break;
		}

		sndVolume = std::max(0.1f, factor) * hemi_factor;

		// Ambient sound
		if (snd_Ambient._feedback())
			snd_Ambient.set_volume(sndVolume);
	}
}

void CEffectRain::InvalidateState()
{
	state = stIdle;
}

void CEffectRain::UpdateTimer(RainTimer& timer, States NewState, bool bNeedRayPick)
{
	float factor = Environment().CurrentEnv->rain_density;
	if (factor > EPS_L)
	{
		// Is raining	
		if (NewState == States::stWorking)
		{
			// Effect is enabled
			Fvector P, D;
			P.set(Device.vCameraPosition);	// Camera position
			D.set(0, 1, 0);					// Direction to sky

			float max_dist = max_distance;
			u16 material = GAMEMTL_NONE_IDX;
			if (!bNeedRayPick || !RayPick(P, D, max_dist, material, collide::rqtBoth))
			{
				// Under the sky
				if (timer.bFirstRainingFrame && !Device.dwPrecacheFrame)
				{
					// First frame
					timer.bFirstRainingFrame = false;
					timer.rainDropTime = timer.rainDropTimeBasic / factor;		// Speed of getting wet
					timer.rainTimestamp = Device.fTimeGlobal;
					if (timer.rainTimer > EPS)
						timer.rainTimestamp += timer.lastRainDuration - timer.rainTimer - std::min(timer.rainDropTime, timer.lastRainDuration);

					timer.lastRainDuration = 0.0f;
				}

				if (timer.rainTimer < 0.0f)
					timer.rainTimer = 0.0f;

				timer.rainTimer = Device.fTimeGlobal - timer.rainTimestamp;
			}
			else
			{
				// Under the cover. But may be it just appeared
				if (timer.rainTimer > EPS)
				{
					// Actor was under the sky recently
					float delta = timer.rainTimer - (Device.fTimeGlobal - timer.previousFrameTime);
					timer.rainTimer = (delta > 0.0f) ? delta : 0.0f;
					if (!timer.bFirstRainingFrame)
					{
						// First update since rain was stopped
						timer.bFirstRainingFrame = true;
						timer.lastRainDuration = Device.fTimeGlobal - timer.rainTimestamp;
					}
				}
			}
		}
		else
		{
			// Effect is disabled, reset all
			timer.bFirstRainingFrame = true;
			timer.lastRainDuration = 0.0f;
			timer.rainTimer = 0.0f;
			timer.rainTimestamp = Device.fTimeGlobal;
		}
		timer.previousFrameTime = Device.fTimeGlobal;
	}
	else
	{
		// No rain. But may be it just stopped
		if (timer.rainTimer > EPS)
		{
			// Yes, it has been raining recently
			// so decrease timer
			float delta = timer.rainTimer - (Device.fTimeGlobal - timer.previousFrameTime);
			timer.rainTimer = (delta > 0.0f) ? delta : 0.0f;
			if (!timer.bFirstRainingFrame)
			{
				// First update since rain was stopped
				timer.bFirstRainingFrame = true;
				timer.lastRainDuration = Device.fTimeGlobal - timer.rainTimestamp;
			}
			timer.previousFrameTime = Device.fTimeGlobal;
		}
	}
}

void CEffectRain::Render()
{
	if (g_pGameLevel)
		m_pRender->Render(*this);
}

// startup _new_ particle system
void CEffectRain::Hit(Item& item)
{
	Particle* P = ParticleAllocate();
	if (!P)
		return;

	const Fsphere &bv_sphere = m_pRender->GetDropBounds();

	P->time						= particles_time;
	P->mXForm.rotateY			(::Random.randF(PI_MUL_2));
	P->mXForm.translate_over	(item.Phit);
	P->mXForm.transform_tiny	(P->bounds.P, bv_sphere.P);
	P->bounds.R					= bv_sphere.R;
}

// Initialize particles pool
void CEffectRain::ParticlePoolCreate()
{
	// Pool
	particle_pool.resize(max_particles);

	for (u32 it = 0; it < particle_pool.size(); it++)
	{
		Particle& P = particle_pool[it];
		P.prev		= it ? (&particle_pool[it - 1]) : nullptr;
		P.next		= (it < (particle_pool.size() - 1)) ? (&particle_pool[it + 1]) : nullptr;
	}
	
	// active and idle lists
	particle_active	= nullptr;
	particle_idle	= &particle_pool.front();
}

// Destroy particles pool
void CEffectRain::ParticlePoolDestroy()
{
	// Active and idle lists
	particle_active	= nullptr;
	particle_idle	= nullptr;
	
	// Pool
	particle_pool.clear();
}

// _delete_ node from _list_
void CEffectRain::ParticleRemove(Particle* P, Particle* &LST)
{
	VERIFY(P);
	Particle* prev = P->prev; P->prev = nullptr;
	Particle* next = P->next; P->next = nullptr;
	if (prev)
		prev->next	= next;

	if (next)
		next->prev	= prev;

	if (LST == P)
		LST = next;
}

// insert node at the top of the head
void CEffectRain::ParticleInsert(Particle* P, Particle* &LST)
{
	VERIFY(P);
	P->prev = 0;
	P->next = LST;
	if (LST)
		LST->prev = P;

	LST = P;
}

// determine size of _list_
int CEffectRain::ParticleListSize(Particle* P)
{
	if (!P)
		return 0;

	int cnt = 0;
	while (P)
	{
		P = P->next;
		cnt += 1;
	}
	return cnt;
}

// alloc node
CEffectRain::Particle* CEffectRain::ParticleAllocate()
{
	Particle* P = particle_idle;
	if (!P)
		return nullptr;

	ParticleRemove(P, particle_idle);
	ParticleInsert(P, particle_active);
	return P;
}

// xr_free node
void CEffectRain::ParticleFree(Particle* P)
{
	ParticleRemove(P, particle_active);
	ParticleInsert(P, particle_idle);
}
