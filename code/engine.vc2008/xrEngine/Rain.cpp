#include "stdafx.h"
#pragma once

#include "Rain.h"
#include "igame_persistent.h"
#include "environment.h"

#ifdef _EDITOR
    #include "ui_toolscustom.h"
#else
    #include "render.h"
	#include "igame_level.h"
	#include "../xrcdb/xr_area.h"
	#include "xr_object.h"
#endif

rain_timer_params* rain_timers = NULL;
Fvector4* rain_params = NULL;

//	Warning: duplicated in dxRainRender
static const int	max_desired_items	= 2500;
static const float	source_radius		= 12.5f;
static const float	source_offset		= 40.f;
static const float	max_distance		= source_offset*1.25f;
static const float	sink_offset			= -(max_distance-source_offset);
static const float	drop_length			= 5.f;
static const float	drop_width			= 0.30f;
static const float	drop_angle			= 70.0f;
static const float	drop_max_angle		= deg2rad(89.f);
static const float	drop_max_wind_vel	= 100.0f;
static const float	drop_speed_min		= 40.f;
static const float	drop_speed_max		= 85.f;

const int	max_particles		= 1000;
const int	particles_cache		= 400;
const float particles_time		= .3f;
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffect_Rain::CEffect_Rain()
{
	state							= stIdle;
	
	snd_Ambient.create				("ambient\\rain",st_Effect,sg_Undefined);

	//	Moced to p_Render constructor
	p_create						();

#ifndef _EDITOR
    if (!rain_timers)
        rain_timers = xr_new<rain_timer_params>();
    if (!rain_params)
        rain_params = xr_new<Fvector4>();
#endif
}

CEffect_Rain::~CEffect_Rain()
{
	snd_Ambient.destroy				();

	// Cleanup
	p_destroy						();

#ifndef _EDITOR
    if (rain_timers)
        xr_delete(rain_timers);
    if (rain_params)
        xr_delete(rain_params);
#endif
}

// Born
void	CEffect_Rain::Born		(Item& dest, float radius)
{
	Fvector		axis;	
    axis.set			(0,-1,0);
	float gust			= g_pGamePersistent->Environment().wind_strength_factor;
	float k				= g_pGamePersistent->Environment().CurrentEnv->wind_velocity*gust/drop_max_wind_vel;
	clamp				(k,0.f,1.f);
	float	pitch		= drop_max_angle*k-PI_DIV_2;
    axis.setHP			(g_pGamePersistent->Environment().CurrentEnv->wind_direction,pitch);
    
	Fvector&	view	= Device.vCameraPosition;
	float		angle	= ::Random.randF	(0,PI_MUL_2);
	float		dist	= ::Random.randF	(); dist = _sqrt(dist)*radius; 
	float		x		= dist*_cos		(angle);
	float		z		= dist*_sin		(angle);
	dest.D.random_dir	(axis,deg2rad(drop_angle));
	dest.P.set			(x+view.x-dest.D.x*source_offset,source_offset+view.y,z+view.z-dest.D.z*source_offset);
//	dest.P.set			(x+view.x,height+view.y,z+view.z);
	dest.fSpeed			= ::Random.randF	(drop_speed_min,drop_speed_max);

	float height		= max_distance;
	RenewItem			(dest,height,RayPick(dest.P,dest.D,height,collide::rqtBoth));
}

BOOL CEffect_Rain::RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt)
{
	collide::rq_result	RQ;
	CObject* E 			= g_pGameLevel->CurrentViewEntity();
	const bool bRes = (bool)g_pGameLevel->ObjectSpace.RayPick( s,d,range,tgt,RQ,E);	
    if (bRes) range 	= RQ.range;
    return bRes;
}

void CEffect_Rain::RenewItem(Item& dest, float height, BOOL bHit)
{
	dest.uv_set			= Random.randI(2);
    if (bHit){
		dest.dwTime_Life= Device.dwTimeGlobal + iFloor(1000.f*height/dest.fSpeed) - Device.dwTimeDelta;
		dest.dwTime_Hit	= Device.dwTimeGlobal + iFloor(1000.f*height/dest.fSpeed) - Device.dwTimeDelta;
		dest.Phit.mad	(dest.P,dest.D,height);
	}else{
		dest.dwTime_Life= Device.dwTimeGlobal + iFloor(1000.f*height/dest.fSpeed) - Device.dwTimeDelta;
		dest.dwTime_Hit	= Device.dwTimeGlobal + iFloor(2*1000.f*height/dest.fSpeed)-Device.dwTimeDelta;
		dest.Phit.set	(dest.P);
	}
}

void	CEffect_Rain::OnFrame	()
{
	if (!g_pGameLevel)			return;

	// Parse states
	float	factor				= g_pGamePersistent->Environment().CurrentEnv->rain_density;
	static float hemi_factor	= 0.f;
	CObject* E 					= g_pGameLevel->CurrentViewEntity();
	if (E&&E->renderable_ROS())
	{
		float* hemi_cube		= E->renderable_ROS()->get_luminocity_hemi_cube();
		float hemi_val			= std::max(hemi_cube[0],hemi_cube[1]);
		hemi_val				= std::max(hemi_val, hemi_cube[2]);
		hemi_val				= std::max(hemi_val, hemi_cube[3]);
		hemi_val				= std::max(hemi_val, hemi_cube[5]);
		
		float f					= hemi_val;
		float t					= Device.fTimeDelta;
		clamp					(t, 0.001f, 1.0f);
		hemi_factor				= hemi_factor*(1.0f-t) + f*t;
	}

	switch (state)
	{
	case stIdle:		
		if (factor<EPS_L)		return;
		state					= stWorking;
		snd_Ambient.play		(0,sm_Looped);
		snd_Ambient.set_position(Fvector().set(0,0,0));
		snd_Ambient.set_range	(source_offset,source_offset*2.f);
	break;
	case stWorking:
		if (factor<EPS_L){
			state				= stIdle;
			snd_Ambient.stop	();
			return;
		}
		break;
	}

	// ambient sound
	if (snd_Ambient._feedback())
		snd_Ambient.set_volume	(std::max(0.1f,factor) * hemi_factor );
}

#ifndef _EDITOR
BOOL rain_timer_params::RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt)
{
    BOOL bRes = TRUE;
    collide::rq_result	RQ;
    CObject* E = g_pGameLevel->CurrentViewEntity();
    bRes = g_pGameLevel->ObjectSpace.RayPick(s, d, range, tgt, RQ, E);
    if (bRes) range = RQ.range;
    return bRes;
}
int rain_timer_params::Update(BOOL state, bool need_raypick)
{
    float	factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    if (factor>EPS_L)
    {
        // is raining	
        if (state)
        {
            // effect is enabled
            Fvector P, D;
            P = Device.vCameraPosition;	// cam position
            D.set(0, 1, 0);				// direction to sky
            float max_dist = max_distance;
            if (!need_raypick || !RayPick(P, D, max_dist, collide::rqtBoth))
            {
                // under the sky
                if (!not_first_frame)
                {
                    // first frame
                    not_first_frame = TRUE;
                    rain_drop_time = rain_drop_time_basic / factor;		// speed of getting wet
                    rain_timestamp = Device.fTimeGlobal;
                    if (rain_timer > EPS)
                        rain_timestamp += last_rain_duration - rain_timer - std::min(rain_drop_time, last_rain_duration);
                    last_rain_duration = 0;
                }
                // проверяем, не отрицателен ли дождевой таймер, если отрицателен - обнуляем
                // такое может быть при первом кадре с дождем, если до этого дождь уже как-то раз был в текущей игровой сессии
                if (rain_timer < 0)
                    rain_timer = 0;
                rain_timer = Device.fTimeGlobal - rain_timestamp;
            }
            else
            {
                // under the cover. but may be it just appear
                if (rain_timer > EPS)
                {
                    // yes, actor was under the sky recently
                    float delta = rain_timer - (Device.fTimeGlobal - previous_frame_time);
                    rain_timer = (delta>0) ? delta : 0;
                    if (not_first_frame)
                    {
                        // first update since rain was stopped
                        not_first_frame = FALSE;
                        last_rain_duration = Device.fTimeGlobal - rain_timestamp;
                    }
                }
            }
        }
        else
        {
            // effect is disabled, reset all
            not_first_frame = FALSE;
            last_rain_duration = 0;
            rain_timer = 0;
            rain_timestamp = Device.fTimeGlobal;
        }
        previous_frame_time = Device.fTimeGlobal;
        timer.set(rain_timer, last_rain_duration, rain_drop_time);
        return IS_RAIN;
    }
    else
    {
        // no rain. but may be it just stop
        if (rain_timer > EPS)
        {
            // yes, it has been raining recently
            // so decrease timer
            float delta = rain_timer - (Device.fTimeGlobal - previous_frame_time);
            rain_timer = (delta>0) ? delta : 0;
            if (not_first_frame)
            {
                // first update since rain was stopped
                not_first_frame = FALSE;
                last_rain_duration = Device.fTimeGlobal - rain_timestamp;
            }
            previous_frame_time = Device.fTimeGlobal;
        }
        timer.set(rain_timer, last_rain_duration, rain_drop_time);
        return NO_RAIN;
    }
}
#endif

//#include "xr_input.h"
void	CEffect_Rain::Render	()
{
	if (g_pGameLevel)
	{
		m_pRender->Render(*this);
	}
}

// startup _new_ particle system
void	CEffect_Rain::Hit		(Fvector& pos)
{
	if (::Random.randI(2))	return;
	Particle*	P	= p_allocate();
	if (!P)	return;

	const Fsphere &bv_sphere = m_pRender->GetDropBounds();

	P->time						= particles_time;
	P->mXForm.rotateY			(::Random.randF(PI_MUL_2));
	P->mXForm.translate_over	(pos);
	P->mXForm.transform_tiny	(P->bounds.P, bv_sphere.P);
	P->bounds.R					= bv_sphere.R;

}

// initialize particles pool
void CEffect_Rain::p_create		()
{
	// pool
	particle_pool.resize	(max_particles);
	for (u32 it=0; it<particle_pool.size(); it++)
	{
		Particle&	P	= particle_pool[it];
		P.prev			= it?(&particle_pool[it-1]):0;
		P.next			= (it<(particle_pool.size()-1))?(&particle_pool[it+1]):0;
	}
	
	// active and idle lists
	particle_active	= 0;
	particle_idle	= &particle_pool.front();
}

// destroy particles pool
void CEffect_Rain::p_destroy	()
{
	// active and idle lists
	particle_active	= 0;
	particle_idle	= 0;
	
	// pool
	particle_pool.clear	();
}

// _delete_ node from _list_
void CEffect_Rain::p_remove	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	Particle*	prev		= P->prev;	P->prev = NULL;
	Particle*	next		= P->next;	P->next	= NULL;
	if (prev) prev->next	= next;
	if (next) next->prev	= prev;
	if (LST==P)	LST			= next;
}

// insert node at the top of the head
void CEffect_Rain::p_insert	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	P->prev					= 0;
	P->next					= LST;
	if (LST)	LST->prev	= P;
	LST						= P;
}

// determine size of _list_
int CEffect_Rain::p_size	(Particle* P)
{
	if (!P)	return 0;
	int cnt = 0;
	while (P)	
	{
		P	=	P->next;
		cnt +=	1;
	}
	return cnt;
}

// alloc node
CEffect_Rain::Particle*	CEffect_Rain::p_allocate	()
{
	Particle*	P			= particle_idle;
	if (!P)				return nullptr;
	p_remove	(P,particle_idle);
	p_insert	(P,particle_active);
	return		P;
}

// xr_free node
void	CEffect_Rain::p_free(Particle* P)
{
	p_remove	(P,particle_active);
	p_insert	(P,particle_idle);
}
