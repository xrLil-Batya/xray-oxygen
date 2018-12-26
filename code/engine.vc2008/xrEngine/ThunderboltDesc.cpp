#include "stdafx.h"
#pragma hdrstop

#include "ThunderboltDesc.h"
#include "LightAnimLibrary.h"

CThunderboltDesc::CThunderboltDesc() :
	m_GradientTop	(nullptr),
	m_GradientCenter(nullptr)
{
}

CThunderboltDesc::~CThunderboltDesc()
{
	m_pRender->DestroyModel		();

	m_GradientTop->m_pFlare->DestroyShader();
	m_GradientCenter->m_pFlare->DestroyShader();

    snd.destroy					();

	xr_delete					(m_GradientTop);
	xr_delete					(m_GradientCenter);
}

void CThunderboltDesc::CreateTopGradient(CInifile& pIni, shared_str const& sect)
{
	m_GradientTop				= xr_new<SFlare>();
	m_GradientTop->shader		= pIni.r_string		(sect, "gradient_top_shader");
	m_GradientTop->texture		= pIni.r_string		(sect, "gradient_top_texture");
	m_GradientTop->fRadius		= pIni.r_fvector2	(sect, "gradient_top_radius");
	m_GradientTop->fOpacity		= pIni.r_float		(sect, "gradient_top_opacity");
	m_GradientTop->m_pFlare->CreateShader(*m_GradientTop->shader, *m_GradientTop->texture);
}

void CThunderboltDesc::CreateCenterGradient(CInifile& pIni, shared_str const& sect)
{
	m_GradientCenter			= xr_new<SFlare>();
	m_GradientCenter->shader	= pIni.r_string		(sect, "gradient_center_shader");
	m_GradientCenter->texture	= pIni.r_string		(sect, "gradient_center_texture");
	m_GradientCenter->fRadius	= pIni.r_fvector2	(sect, "gradient_center_radius");
	m_GradientCenter->fOpacity	= pIni.r_float		(sect, "gradient_center_opacity");
	m_GradientCenter->m_pFlare->CreateShader(*m_GradientCenter->shader, *m_GradientCenter->texture);
}

void CThunderboltDesc::Load(CInifile& pIni, shared_str const& sect)
{
	CreateTopGradient			(pIni, sect);
	CreateCenterGradient		(pIni, sect);

	name						= sect;
	color_anim					= LALib.FindItem(pIni.r_string(sect, "color_anim"));
	VERIFY						(color_anim);
	color_anim->fFPS			= (float)color_anim->iFrameCount;

    // models
	LPCSTR m_name;
	m_name						= pIni.r_string(sect, "lightning_model");
	m_pRender->CreateModel		(m_name);

    // sound
	m_name						= pIni.r_string(sect,"sound");
	if (m_name && m_name[0])
		snd.create(m_name, st_Effect, sg_Undefined);
}
