#include "stdafx.h"
#pragma once

#ifndef _EDITOR
    #include "Render.h"
	#include "IGame_Level.h"
	#include "../xrCDB/xr_area.h"
	#include "xr_object.h"
#else
	#include "ui_toolscustom.h"
#endif   

#include "Thunderbolt.h"
#include "ThunderboltDesc.h"
#include "ThunderboltCollection.h"
#include "LightAnimLibrary.h"

#include "IGame_Persistent.h"

CEffect_Thunderbolt::CEffect_Thunderbolt()
{
	current				= nullptr;
	life_time			= 0.0f;
    state				= stIdle;
    next_lightning_time = 0.0f;
    bEnabled			= FALSE;
}

CEffect_Thunderbolt::~CEffect_Thunderbolt()
{
	for (CThunderboltCollection* pTBC : collection)
		xr_delete(pTBC);

	collection.clear();
}

shared_str CEffect_Thunderbolt::AppendDef(CEnvironment& environment, CInifile* pIni, CInifile* thunderbolts, LPCSTR sect)
{
	if (!sect || (0 == sect[0]))
		return "";

	for (CThunderboltCollection* pTBC : collection)
	{
		if (0 == xr_strcmp(pTBC->section.c_str(), sect))
			return pTBC->section;
	}

	collection.push_back(environment.ThunderboltCollection(pIni, thunderbolts, sect));
	return collection.back()->section;
}

bool CEffect_Thunderbolt::RayPick(const Fvector& s, const Fvector& d, float& dist)
{
	bool bRes = true;
#ifdef _EDITOR
	bRes = Tools->RayPick(s, d, dist, 0, 0);
#else
	collide::rq_result RQ;
	CObject* pCurrViewEntity = g_pGameLevel->CurrentViewEntity();
	bRes = g_pGameLevel->ObjectSpace.RayPick(s, d, dist, collide::rqtBoth, RQ, pCurrViewEntity);

	if (bRes)
		dist = RQ.range;
	else
	{
		Fvector N = { 0.f, -1.f, 0.f };
		Fvector P = { 0.f,  0.f, 0.f };
		Fplane PL; PL.build(P, N);
		float dst = dist;
		if (PL.intersectRayDist(s, d, dst) && (dst <= dist))
		{
			dist = dst;
			return true;
		}
		return false;
	}
#endif
    return bRes;
}
#define FAR_DIST Environment().CurrentEnv->far_plane

void CEffect_Thunderbolt::Bolt(shared_str id, float period, float lt)
{
	VERIFY					(id.size());
	state 		            = stWorking;
	life_time				= lt + Random.randF(-lt * 0.5f, lt * 0.5f);
    current_time            = 0.0f;

    current		            = Environment().ThunderboltCollection(collection, id)->GetRandomDesc(); VERIFY(current);

    Fmatrix XF, S;
    Fvector pos, dev;
    float sun_h, sun_p; 
	CEnvironment&			environment = Environment();
    environment.CurrentEnv->sun_dir.getHP(sun_h, sun_p);
	float alt				= environment.p_var_alt;//Random.randF(environment.p_var_alt.x,environment.p_var_alt.y);
	float lng				= Random.randF(sun_h - environment.p_var_long + PI, sun_h + environment.p_var_long + PI);
	float dist				= Random.randF(FAR_DIST*environment.p_min_dist, FAR_DIST*0.95f);
    current_direction.setHP	(lng, alt);
	pos.mad					(Device.vCameraPosition, current_direction, dist);
	dev.x					= Random.randF(-environment.p_tilt, environment.p_tilt);
    dev.y		            = Random.randF(0, PI_MUL_2);
    dev.z		            = Random.randF(-environment.p_tilt, environment.p_tilt);
    XF.setXYZi	            (dev);               

	Fvector light_dir		= { 0.f, -1.f, 0.f };
    XF.transform_dir		(light_dir);
    lightning_size			= FAR_DIST*2.f;
    RayPick					(pos,light_dir, lightning_size);

    lightning_center.mad	(pos,light_dir, lightning_size*0.5f);

    S.scale					(lightning_size, lightning_size, lightning_size);
    XF.translate_over		(pos);
    current_xform.mul_43	(XF, S);

    float next_v			= Random.randF();

	if (next_v < environment.p_second_prop)
	{
		next_lightning_time = Device.fTimeGlobal + lt + EPS_L;
	}
	else
	{
		next_lightning_time = Device.fTimeGlobal + period + Random.randF(-period * 0.3f, period*0.3f);
		current->snd.play_no_feedback(0, 0, dist / 300.f, &pos, 0, 0, &Fvector2().set(dist / 2, dist*2.f));
	}
	current_direction.invert();	// for env-sun
}

void CEffect_Thunderbolt::OnFrame(shared_str id, float period, float duration)
{
	bool enabled = (id.size() > 0);

	if (bEnabled != enabled)
	{
    	bEnabled = enabled;
		next_lightning_time = Device.fTimeGlobal + period + Random.randF(-period * 0.5f, period*0.5f);
    }
	else if (bEnabled && (Device.fTimeGlobal > next_lightning_time) && (enabled && state == stIdle))
	{
		Bolt(id, period, duration);
	}

	if (state == stWorking)
	{
		if (current_time > life_time)
			state = stIdle;

		current_time += Device.fTimeDelta;
		Fvector fClr;
		int frame;
		u32 uClr = current->color_anim->CalculateRGB(current_time / life_time, frame);
		fClr.set(
			clampr(float(color_get_R(uClr) / 255.f), 0.0f, 1.0f),
			clampr(float(color_get_G(uClr) / 255.f), 0.0f, 1.0f),
			clampr(float(color_get_B(uClr) / 255.f), 0.0f, 1.0f)
		);

		lightning_phase = 1.5f*(current_time / life_time);
		clamp(lightning_phase, 0.f, 1.f);

		CEnvironment& environment = Environment();

		Fvector& sky_color = environment.CurrentEnv->sky_color;
		sky_color.mad(fClr, environment.p_sky_color);
		clamp(sky_color.x, 0.0f, 1.0f);
		clamp(sky_color.y, 0.0f, 1.0f);
		clamp(sky_color.z, 0.0f, 1.0f);

		environment.CurrentEnv->sun_color.mad(fClr, environment.p_sun_color);
		environment.CurrentEnv->fog_color.mad(fClr, environment.p_fog_color);

		R_ASSERT(_valid(current_direction));
		Environment().CurrentEnv->sun_dir = current_direction;
		VERIFY2(Environment().CurrentEnv->sun_dir.y < 0, "Invalid sun direction settings while CEffect_Thunderbolt");
	}
}

void CEffect_Thunderbolt::Render()
{                  
	if (state == stWorking)
		m_pRender->Render(*this);
}
