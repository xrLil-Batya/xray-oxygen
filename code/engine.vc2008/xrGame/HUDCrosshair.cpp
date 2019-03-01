// HUDCrosshair.cpp:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "actor.h"
#include "HUDCrosshair.h"
#include "../xrUICore/ui_base.h"

CHUDCrosshair::CHUDCrosshair	()
{
	hShader->create				("hud\\crosshair");
	radius = 0;
}


CHUDCrosshair::~CHUDCrosshair	()
{
}

void CHUDCrosshair::Load		()
{
	//все размеры в процентах от длины экрана
	//длина крестика 
	cross_length_perc = pSettings->r_float (HUD_CURSOR_SECTION, "cross_length");
	min_radius_perc = pSettings->r_float (HUD_CURSOR_SECTION, "min_radius");
	max_radius_perc = pSettings->r_float (HUD_CURSOR_SECTION, "max_radius");
	cross_color = pSettings->r_fcolor (HUD_CURSOR_SECTION, "cross_color").get();
	is_enabled = READ_IF_EXISTS(pSettings, r_bool, HUD_CURSOR_SECTION, "cross_enabled", true);
}

//выставляет radius от min_radius до max_radius
void CHUDCrosshair::SetDispersion	(float disp)
{ 
	Fvector4 r;
	Fvector R			= { VIEWPORT_NEAR*_sin(disp), 0.f, VIEWPORT_NEAR };
	Device.mProject.transform	(r,R);

	Fvector2		scr_size;
	scr_size.set	(float(::Render->getTarget()->get_width()), float(::Render->getTarget()->get_height()));
	target_radius		= _abs(r.x)*scr_size.x/2.0f;
}

#ifdef DEBUG
void CHUDCrosshair::SetFirstBulletDispertion(float fbdisp)
{
	Fvector4 r;
	Fvector R			= { VIEWPORT_NEAR*_sin(fbdisp), 0.f, VIEWPORT_NEAR };
	Device.mProject.transform	(r,R);

	Fvector2		scr_size;
	scr_size.set	(float(::Render->getTarget()->get_width()), float(::Render->getTarget()->get_height()));
	fb_radius		= _abs(r.x)*scr_size.x/2.0f;
}

BOOL	g_bDrawFirstBulletCrosshair = FALSE;

void CHUDCrosshair::OnRenderFirstBulletDispertion()
{
	VERIFY			(g_bRendering);
	Fvector2		center;
	Fvector2		scr_size;
	scr_size.set	(float(::Render->getTarget()->get_width()), float(::Render->getTarget()->get_height()));
	center.set		(scr_size.x/2.0f, scr_size.y/2.0f);

	UIRender->StartPrimitive		(10, IUIRender::ptLineList, UI().m_currentPointType);

	u32	fb_cross_color				= color_rgba(255, 0, 0, 255); //red
	

	float cross_length				= /*cross_length_perc*/0.008f*scr_size.x;
	float min_radius				= min_radius_perc*scr_size.x;
	float max_radius				= max_radius_perc*scr_size.x;

	clamp							(target_radius , min_radius, max_radius);

	float x_min						= min_radius + fb_radius;
	float x_max						= x_min + cross_length;

	float y_min						= x_min;
	float y_max						= x_max;

	// 0
	UIRender->PushPoint(center.x,			center.y + y_min,	0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x,			center.y + y_max,	0, fb_cross_color, 0,0);
	// 1
	UIRender->PushPoint(center.x,			center.y - y_min,	0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x,			center.y - y_max,	0, fb_cross_color, 0,0);
	// 2
	UIRender->PushPoint(center.x + x_min,	center.y,			0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x + x_max,	center.y,			0, fb_cross_color, 0,0);
	// 3
	UIRender->PushPoint(center.x - x_min,	center.y,			0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x - x_max,	center.y,			0, fb_cross_color, 0,0);
	
	// point
	UIRender->PushPoint(center.x-0.5f,		center.y,			0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x+0.5f,		center.y,			0, fb_cross_color, 0,0);


	// render	
	UIRender->SetShader						(*hShader);
	UIRender->FlushPrimitive				();
}
#endif
#include "HudManager.h"
#include "inventory.h"
#include "items/weapon.h"
extern ENGINE_API BOOL g_bRendering; 
void CHUDCrosshair::OnRender ()
{
	if (psActorFlags.test(AF_HARDCORE)) return;

	VERIFY			(g_bRendering);
	Fvector2		center, scr_size;
	Fvector			result;
	Fvector4		v_res;
	float			x, y;
	scr_size.set	(float(::Render->getTarget()->get_width()), float(::Render->getTarget()->get_height()));

	CWeapon				*weapon = smart_cast<CWeapon*>(Actor()->inventory().ActiveItem());
	CCameraBase			*pCam = Actor()->cam_Active();
	float dist			= HUD().GetCurrentRayQuery().range*1.2f;

	if (weapon && psActorFlags.test(AF_CROSSHAIR_COLLIDE) && !psActorFlags.test(AF_CROSSHAIR_INERT))
	{
		result = weapon->get_LastFP();
		result.add(Fvector(Device.vCameraDirection).mul(dist));
	}

	if (psActorFlags.test(AF_CROSSHAIR_INERT) && !psActorFlags.test(AF_CROSSHAIR_COLLIDE))
	{
		result = pCam->vPosition;
		result.add(Fvector(pCam->vDirection).mul(dist));
	}

	Device.mFullTransform.transform(v_res, result);

	x = (1.f + v_res.x) / 2.f * (Device.dwWidth);
	y = (1.f - v_res.y) / 2.f * (Device.dwHeight);

	if ((psActorFlags.test(AF_CROSSHAIR_INERT) || psActorFlags.test(AF_CROSSHAIR_COLLIDE)) && !(psActorFlags.test(AF_CROSSHAIR_INERT) && psActorFlags.test(AF_CROSSHAIR_COLLIDE)))
		center.set		(x, y);
	else
		center.set(scr_size.x / 2.0f, scr_size.y / 2.0f);

	UIRender->StartPrimitive		(10, IUIRender::ptLineList, UI().m_currentPointType);
	

	float cross_length					= cross_length_perc*scr_size.x;
	float min_radius					= min_radius_perc*scr_size.x;
	float max_radius					= max_radius_perc*scr_size.x;

	clamp								(target_radius , min_radius, max_radius);

	float x_min							= min_radius + radius;
	float x_max							= x_min + cross_length;

	// 0
	UIRender->PushPoint(center.x,			center.y + x_min,	0, cross_color, 0.0f, 0.0f);
	UIRender->PushPoint(center.x,			center.y + x_max,	0, cross_color, 0.0f, 0.0f);
	// 1
	UIRender->PushPoint(center.x,			center.y - x_min,	0, cross_color, 0.0f, 0.0f);
	UIRender->PushPoint(center.x,			center.y - x_max,	0, cross_color, 0.0f, 0.0f);
	// 2
	UIRender->PushPoint(center.x + x_min,	center.y,			0, cross_color, 0.0f, 0.0f);
	UIRender->PushPoint(center.x + x_max,	center.y,			0, cross_color, 0.0f, 0.0f);
	// 3
	UIRender->PushPoint(center.x - x_min,	center.y,			0, cross_color, 0.0f, 0.0f);
	UIRender->PushPoint(center.x - x_max,	center.y,			0, cross_color, 0.0f, 0.0f);
	
	// point
	UIRender->PushPoint(center.x-0.5f,		center.y,			0, cross_color, 0.0f, 0.0f);
	UIRender->PushPoint(center.x+0.5f,		center.y,			0, cross_color, 0.0f, 0.0f);


	// render	
	UIRender->SetShader(*hShader);
	UIRender->FlushPrimitive		();


	//here was crosshair innertion emulation
	if(!fsimilar(target_radius,radius))
		radius = target_radius;
	
#ifdef DEBUG
	if (g_bDrawFirstBulletCrosshair)
		OnRenderFirstBulletDispertion();
#endif
}