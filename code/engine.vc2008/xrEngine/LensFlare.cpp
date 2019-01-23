#include "stdafx.h"
#pragma hdrstop

#include "LensFlare.h"
#include "igame_persistent.h"
#include "Environment.h"
#include "Bone.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xrCDB/cl_intersect.h"
#include "../xrServerEntities/object_broker.h"

#ifdef _EDITOR
    #include "ui_toolscustom.h"
    #include "ui_main.h"
#else
	#include "xr_object.h"
	#include "IGame_Level.h"
	#include "CameraManager.h"
#endif

#define FAR_DIST Environment().CurrentEnv->far_plane

#define BLEND_INC_SPEED 8.0f
#define BLEND_DEC_SPEED 4.0f

CLensFlare::CLensFlare()
{
	// Device
	dwFrame			= 0xfffffffe;

	fBlend			= 0.f;

    LightColor.set	(0xFFFFFFFF);
	fGradientValue	= 0.f;

	m_Current		= 0;
    m_State			= lfsNone;
    m_StateBlend	= 0.f;

#ifndef _EDITOR
	for (int i = 0; i < MAX_RAYS; ++i)
	{
		m_ray_cache[i].verts[0].set(0, 0, 0);
		m_ray_cache[i].verts[1].set(0, 0, 0);
		m_ray_cache[i].verts[2].set(0, 0, 0);
	}
#endif

	OnDeviceCreate();	
}

CLensFlare::~CLensFlare()
{
	OnDeviceDestroy();
	delete_data(m_Palette);	
}

#ifndef _EDITOR
struct STranspParam
{
	Fvector				P;
	Fvector				D;
	float				f;
	collide::ray_cache*	pray_cache;
	float				vis;
	float				vis_threshold;
	STranspParam		(collide::ray_cache	*p, const Fvector& _P, const Fvector& _D, float _f, float _vis_threshold):P(_P),D(_D),f(_f),pray_cache(p),vis(1.f),vis_threshold(_vis_threshold){}
};

IC bool MaterialCallback(collide::rq_result& result, LPVOID params)
{
	STranspParam* fp = (STranspParam*)params;
	float vis = 1.0f;
	if (result.O)
	{
		vis = 0.0f;
		IKinematics* pK = PKinematics(result.O->renderable.visual);
		if (pK && (result.element > 0))
			vis = g_pGamePersistent->MtlTransparent(pK->LL_GetData(u16(result.element)).game_mtl_idx);
	}
	else
	{
		CDB::TRI* T = g_pGameLevel->ObjectSpace.GetStaticTris() + result.element;
		vis = g_pGamePersistent->MtlTransparent(T->material);
		if (fis_zero(vis))
		{
			Fvector* V = g_pGameLevel->ObjectSpace.GetStaticVerts();
			fp->pray_cache->set(fp->P, fp->D, fp->f, TRUE);
			fp->pray_cache->verts[0].set(V[T->verts[0]]);
			fp->pray_cache->verts[1].set(V[T->verts[1]]);
			fp->pray_cache->verts[2].set(V[T->verts[2]]);
		}
	}
	fp->vis *=vis;
	return (fp->vis > fp->vis_threshold);;
}
#endif

IC void	BlendLerp(float& cur, float tgt, float speed, float dt)
{
	float diff = tgt - cur;
	float diff_a = _abs(diff);
	if (diff_a < EPS_S)
		return;

	float mot = speed*dt;
	if (mot > diff_a)
		mot = diff_a;

	cur += (diff/diff_a)*mot;
}

#if 0
static LPCSTR state_to_string (const CLensFlare::LFState &state)
{
	switch (state) {
		case CLensFlare::lfsNone : return("none");
		case CLensFlare::lfsIdle : return("idle");
		case CLensFlare::lfsHide : return("hide");
		case CLensFlare::lfsShow : return("show");
		default : NODEFAULT;
	}
#ifdef DEBUG
	return			(0);
#endif // DEBUG
}
#endif

static Fvector2 RayDeltas[CLensFlare::MAX_RAYS] = 
{
	{ 0,		0 },
	{ 1,		0 },
	{ -1,		0 },
	{ 0,		-1 },
	{ 0,		1 },
};

void CLensFlare::OnFrame(shared_str id)
{
	if (dwFrame == Device.dwFrame)
		return;
#ifndef _EDITOR
	if (!g_pGameLevel)
		return;
#endif
	dwFrame			= Device.dwFrame;

	R_ASSERT		(_valid(Environment().CurrentEnv->sun_dir));
	vSunDir.mul		(Environment().CurrentEnv->sun_dir, -1.0f);
	R_ASSERT		(_valid(vSunDir));

	// color
    float tf		= Environment().fTimeFactor;
    Fvector& c		= Environment().CurrentEnv->sun_color;
	LightColor.set	(c.x, c.y, c.z, 1.0f);

	CLensFlareDescriptor* desc = id.size() ? Environment().AddFlare(m_Palette, id) : nullptr;

//	LFState previous_state = m_State;
    switch(m_State)
	{
    case lfsNone:
	{
		m_State = lfsShow;
		m_Current = desc;
	}
	break;
    case lfsIdle:
	{
		if (desc != m_Current)
			m_State = lfsHide;
	}
	break;
    case lfsShow:
	{
		m_StateBlend = m_Current ? (m_StateBlend + m_Current->m_StateBlendUpSpeed * Device.fTimeDelta * tf) : 1.0f + EPS;
		if (m_StateBlend >= 1.f)
			m_State = lfsIdle;
	}
    break;
    case lfsHide: 
	{
		m_StateBlend = m_Current ? (m_StateBlend - m_Current->m_StateBlendDnSpeed * Device.fTimeDelta * tf) : 0.0f - EPS;
		if (m_StateBlend <= 0.f)
		{
			m_State = lfsShow;
			m_Current = desc;
			m_StateBlend = m_Current ? m_Current->m_StateBlendUpSpeed * Device.fTimeDelta * tf : 0.0f;
		}
	}
    break;
    }
//	Msg("%6d : [%s] -> [%s]", Device.dwFrame, state_to_string(previous_state), state_to_string(m_State));
    clamp(m_StateBlend,0.f,1.f);

	if (!m_Current || (LightColor.magnitude_rgb() == 0.f))
	{
		bRender = false;
		return;
	}

	// Compute center and axis of flares
	float fDot;

	Fvector vecPos;

	Fmatrix	matEffCamPos;
	matEffCamPos.identity();
	// Calculate our position and direction

	matEffCamPos.i.set(Device.vCameraRight);
	matEffCamPos.j.set(Device.vCameraTop);
	matEffCamPos.k.set(Device.vCameraDirection);
	vecPos.set(Device.vCameraPosition);

	vecDir.set(0.0f, 0.0f, 1.0f);
	matEffCamPos.transform_dir(vecDir);
	vecDir.normalize();

	// Figure out of light (or flare) might be visible
	vecLight.set(vSunDir);
	vecLight.normalize();

	fDot = vecLight.dotproduct(vecDir);

	if (fDot <= 0.01f)
	{
		bRender = false;
		return;
	}
	else
		bRender = true;

	// Calculate the point directly in front of us, on the far clip plane
	float fDistance = FAR_DIST * 0.75f;
	vecCenter.mul(vecDir, fDistance);
	vecCenter.add(vecPos);
	// Calculate position of light on the far clip plane
	vecLight.mul(fDistance / fDot);
	vecLight.add(vecPos);
	// Compute axis which goes from light through the center of the screen
	vecAxis.sub(vecLight, vecCenter);

	// Figure out if light is behind something else
	vecX.set(1.0f, 0.0f, 0.0f);
	matEffCamPos.transform_dir(vecX);
	vecX.normalize();
	R_ASSERT(_valid(vecX));

	vecY.crossproduct(vecX, vecDir);
	R_ASSERT(_valid(vecY));

#ifdef _EDITOR
	float dist = UI->ZFar();
	if (Tools->RayPick(Device.m_Camera.GetPosition(), vSunDir, dist))
		fBlend = fBlend - BLEND_DEC_SPEED * Device.fTimeDelta;
	else
		fBlend = fBlend + BLEND_INC_SPEED * Device.fTimeDelta;
#else

	// Side vectors to bend normal.
	Fvector vecSx;
	Fvector vecSy;

	//float fScale = m_Current->m_Source.fRadius * vSunDir.magnitude();
	//float fScale = m_Current->m_Source.fRadius;
	// HACK: it must be read from the weather!
	float fScale = 0.02f;

	vecSx.mul(vecX, fScale);
	vecSy.mul(vecY, fScale);

	// Ignore actor's model in RayQuery when 1st person view is active
	// to avoid sun flares flickering. In 3rd person view we must take it into account
	// to prevent sun flares appearing through actor.
	// #TODO: RZ to self: check if hud model covers sun and handle it properly
	CObject* pObjIgnore = nullptr;
	CObject* pCurrViewentity = g_pGameLevel->CurrentViewEntity();
	CCameraManager& camManager = g_pGameLevel->Cameras();
	if (pCurrViewentity && camManager.Parent() == pCurrViewentity && camManager.Style() == ECameraStyle::csFirstEye)
		pObjIgnore = pCurrViewentity;

	R_ASSERT				(_valid(vSunDir));
	STranspParam TP			(&m_ray_cache[0], Device.vCameraPosition, vSunDir, 1000.f, EPS_L);

	R_ASSERT				(_valid(TP.P));
	R_ASSERT				(_valid(TP.D));
	collide::ray_defs RD	(TP.P, TP.D, TP.f, CDB::OPT_CULL, collide::rqtBoth);
	float fVisResult = 0.0f;

	for (int i = 0; i < MAX_RAYS; ++i)
	{
		TP.D = vSunDir;
		TP.D.add(Fvector().mul(vecSx, RayDeltas[i].x));
		TP.D.add(Fvector().mul(vecSy, RayDeltas[i].y));
		R_ASSERT		(_valid(TP.D));
		TP.pray_cache	= &(m_ray_cache[i]);
		TP.vis			= 1.0f;
		RD.dir			= TP.D;

		if (m_ray_cache[i].result&&m_ray_cache[i].similar(TP.P,TP.D,TP.f))
		{
			// similar with previous query == 0
			TP.vis				= 0.f;
		}
		else
		{
			float _u,_v,_range;
			if (CDB::TestRayTri(TP.P,TP.D,m_ray_cache[i].verts,_u,_v,_range,false)&&(_range>0 && _range<TP.f))
			{
				TP.vis = 0.f;
			}
			else
			{
				// cache outdated. real query.
				r_dest.r_clear();
				if (g_pGameLevel->ObjectSpace.RayQuery(r_dest, RD, (collide::rq_callback*)MaterialCallback, &TP, nullptr, pObjIgnore))
					m_ray_cache[i].result = FALSE;
			}
		}
		fVisResult += TP.vis;
	}
	fVisResult *= (1.0f / MAX_RAYS);

	BlendLerp(fBlend, fVisResult, BLEND_DEC_SPEED, Device.fTimeDelta);
#endif
	clamp(fBlend, 0.0f, 1.0f);

	// gradient
	if (m_Current->m_Flags.is(CLensFlareDescriptor::flGradient))
    {
		Fvector scr_pos;
		Device.mFullTransform.transform(scr_pos, vecLight);
		float kx = 1.0f, ky = 1.0f;
		float sun_blend		= 0.5f;
		float sun_max		= 2.5f;
		scr_pos.y		   *= -1.0f;

		if (_abs(scr_pos.x) > sun_blend)
			kx = ((sun_max - (float)_abs(scr_pos.x))) / (sun_max - sun_blend);

		if (_abs(scr_pos.y) > sun_blend)
			ky = ((sun_max - (float)_abs(scr_pos.y))) / (sun_max - sun_blend);

		if (!((_abs(scr_pos.x) > sun_max) || (_abs(scr_pos.y) > sun_max)))
		{
			float op = m_StateBlend * m_Current->m_Gradient.fOpacity;
			fGradientValue = kx * ky *  op * fBlend;
		}
		else
			fGradientValue	= 0;
	}
}

void CLensFlare::Render(bool bSun, bool bFlares, bool bGradient)
{
	if (!bRender)
		return;

	if (!m_Current)
		return;

	VERIFY(m_Current);

	m_pRender->Render(*this, bSun, bFlares, bGradient);
}

shared_str CLensFlare::AppendDef(CEnvironment& environment, CInifile* pIni, LPCSTR sect)
{
	if (!sect || (0 == sect[0]))
		return "";

	for (auto it = m_Palette.begin(); it != m_Palette.end(); it++)
	{
		if (0 == xr_strcmp(*(*it)->section, sect))
			return sect;
	}

	environment.AddFlare(m_Palette, sect); 
    return sect;
}

void CLensFlare::OnDeviceCreate()
{
	// VS
	m_pRender->OnDeviceCreate();

	// Palette
	for (CLensFlareDescriptor* pFlareDesc : m_Palette)
		pFlareDesc->OnDeviceCreate();
}

void CLensFlare::OnDeviceDestroy()
{
	// palette
	for (CLensFlareDescriptor* pFlareDesc : m_Palette)
		pFlareDesc->OnDeviceDestroy();

	// VS
	m_pRender->OnDeviceDestroy();
}
