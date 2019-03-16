#include "stdafx.h"
#pragma hdrstop

#include "LensFlareDescriptor.h"
#include "../Include/xrRender/LensFlareRender.h"

CLensFlareDescriptor::CLensFlareDescriptor()
{ 
	m_Flags.zero(); 
	section = 0; 
	m_StateBlendUpSpeed = 0.1f;
	m_StateBlendDnSpeed = 0.1f;
}

void CLensFlareDescriptor::SetSource(float fRadius, BOOL ign_color, LPCSTR tex_name, LPCSTR sh_name)
{
	m_Source.fRadius	= fRadius;
    m_Source.shader		= sh_name;
    m_Source.texture	= tex_name;
    m_Source.ignore_color=ign_color;
}

void CLensFlareDescriptor::SetGradient(float fMaxRadius, float fOpacity, LPCSTR tex_name, LPCSTR sh_name)
{
	m_Gradient.fRadius	= fMaxRadius;
	m_Gradient.fOpacity	= fOpacity;
    m_Gradient.shader	= sh_name;
    m_Gradient.texture	= tex_name;
}

void CLensFlareDescriptor::AddFlare(float fRadius, float fOpacity, float fPosition, LPCSTR tex_name, LPCSTR sh_name)
{
	SFlare F;
	F.fRadius	= fRadius;
	F.fOpacity	= fOpacity;
    F.fPosition	= fPosition;
    F.shader	= sh_name;
    F.texture	= tex_name;
	m_Flares.push_back	(F);
}

void CLensFlareDescriptor::Load(CInifile* pIni, LPCSTR sect)
{
	section = sect;
	m_Flags.set(flSource, pIni->r_bool(sect, "sun"));
	if (m_Flags.is(flSource))
	{
		LPCSTR S	= pIni->r_string(sect, "sun_shader");
		LPCSTR T	= pIni->r_string(sect, "sun_texture");
		float r		= pIni->r_float	(sect, "sun_radius");
		BOOL i		= pIni->r_bool	(sect, "sun_ignore_color");
		SetSource(r, i, T, S);
	}

	m_Flags.set(flFlare, pIni->r_bool(sect, "flares"));
	if (m_Flags.is(flFlare))
	{
		LPCSTR S	= pIni->r_string(sect, "flare_shader");
		LPCSTR T	= pIni->r_string(sect, "flare_textures");
		LPCSTR R	= pIni->r_string(sect, "flare_radius");
		LPCSTR O	= pIni->r_string(sect, "flare_opacity");
		LPCSTR P	= pIni->r_string(sect, "flare_position");

		u32 tcnt = _GetItemCount(T);
		string256 name = {0};
		for (u32 i = 0; i < tcnt; ++i)
		{
			_GetItem(R, i, name); float r = (float)atof(name);
			_GetItem(O, i, name); float o = (float)atof(name);
			_GetItem(P, i, name); float p = (float)atof(name);
			_GetItem(T, i, name);
			AddFlare(r, o, p, name, S);
		}
	}
	m_Flags.set(flGradient, CInifile::IsBOOL(pIni->r_string(sect, "gradient")));
	if (m_Flags.is(flGradient))
	{
		LPCSTR S	= pIni->r_string(sect, "gradient_shader");
		LPCSTR T	= pIni->r_string(sect, "gradient_texture");
		float r		= pIni->r_float	(sect, "gradient_radius");
		float o		= pIni->r_float	(sect, "gradient_opacity");
		SetGradient(r, o, T, S);
	}
	m_StateBlendUpSpeed = 1.0f / (std::max(pIni->r_float(sect, "blend_rise_time"), 0.0f) + EPS_S);
	m_StateBlendDnSpeed = 1.0f / (std::max(pIni->r_float(sect, "blend_down_time"), 0.0f) + EPS_S);

	OnDeviceCreate();
}

void CLensFlareDescriptor::OnDeviceCreate()
{
	// Shaders
	m_Gradient.m_pRender->CreateShader	(*m_Gradient.shader,*m_Gradient.texture);
	m_Source.m_pRender->CreateShader	(*m_Source.shader,*m_Source.texture);
	for (CLensFlareDescriptor::SFlare& flareDesc : m_Flares)
		flareDesc.m_pRender->CreateShader(flareDesc.shader.c_str(), flareDesc.texture.c_str());
}

void CLensFlareDescriptor::OnDeviceDestroy()
{
	// shaders
	m_Gradient.m_pRender->DestroyShader	();
	m_Source.m_pRender->DestroyShader	();
	for (CLensFlareDescriptor::SFlare& flareDesc : m_Flares)
		flareDesc.m_pRender->DestroyShader();
}