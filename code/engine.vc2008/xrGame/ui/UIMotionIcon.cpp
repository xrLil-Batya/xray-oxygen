#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UIMotionIcon.h"
#include "UIXmlInit.h"
#include "../FrayBuildConfig.hpp"

const LPCSTR MOTION_ICON_XML = "ui_HUD.xml";

CUIMotionIcon* g_pMotionIcon = NULL;

CUIMotionIcon::CUIMotionIcon()
{
	g_pMotionIcon	= this;
	m_bchanged		= false;
	m_luminosity	= 0.0f;
	cur_pos			= 0.f;
}

CUIMotionIcon::~CUIMotionIcon()
{
	g_pMotionIcon	= NULL;
}

void CUIMotionIcon::ResetVisibility()
{
	m_npc_visibility.clear	();
	m_bchanged				= true;
}

void CUIMotionIcon::Init(Frect const& zonemap_rect)
{
	CUIXml						uiXml;
	uiXml.Load					(CONFIG_PATH, UI_PATH, MOTION_ICON_XML);

	CUIXmlInit					xml_init;

	xml_init.InitWindow			(uiXml, "window", 0, this);
	float rel_sz				= uiXml.ReadAttribFlt("window", 0, "rel_size", 1.0f);
	Fvector2					sz;
	Fvector2					pos;
	zonemap_rect.getsize		(sz);

	pos.set						(sz.x/2.0f, sz.y/2.0f);
	SetWndSize					(sz);
	SetWndPos					(pos);

	float k = UI().get_current_kx();
	sz.mul						(rel_sz*k);

	AttachChild					(&m_luminosity_shape);
	xml_init.InitProgressShape	(uiXml, "luminosity_shape", 0, &m_luminosity_shape);		
	m_luminosity_shape.SetWndSize(sz);
	m_luminosity_shape.SetWndPos(pos);
	
	m_luminosity_shape.SetPos(0.f);	
	
#ifdef MINMAP_BAR_HUD
	AttachChild					(&m_luminosity_bar);
	xml_init.InitProgressBar	(uiXml, "luminosity_bar", 0, &m_luminosity_bar);	
#endif

	AttachChild					(&m_noise_shape);
	xml_init.InitProgressShape	(uiXml, "noise_shape", 0, &m_noise_shape);
	m_noise_shape.SetWndSize	(sz);
	m_noise_shape.SetWndPos	(pos);
	
#ifdef MINMAP_BAR_HUD
	AttachChild					(&m_noise_bar);
	xml_init.InitProgressBar	(uiXml, "noise_bar", 0, &m_noise_bar);	
#endif
}
void CUIMotionIcon::SetNoise(float Pos)
{
	Pos	= clampr(Pos, 0.f, 100.f);
	m_noise_shape.SetPos(Pos/100.f);

#ifdef MINMAP_BAR_HUD
	Pos	= clampr(Pos, m_noise_bar.GetRange_min(), m_noise_bar.GetRange_max());
	m_noise_bar.SetProgressPos(Pos);
#endif
}

void CUIMotionIcon::SetLuminosity(float Pos)
{
	m_luminosity	= Pos;

#ifdef MINMAP_BAR_HUD
	Pos						= clampr(Pos, m_luminosity_bar.GetRange_min(), m_luminosity_bar.GetRange_max());
	m_luminosity			= Pos;
#endif
}

void CUIMotionIcon::Draw()
{
	inherited::Draw();
}

void CUIMotionIcon::Update()
{
	if(m_bchanged){
		m_bchanged = false;
		if( m_npc_visibility.size() )
		{
			std::sort		(m_npc_visibility.begin(), m_npc_visibility.end());
			SetLuminosity	(m_npc_visibility.back().value);
		}
		else
			SetLuminosity	(0.f);
	}
	inherited::Update();
	
	//m_luminosity_progress 
#ifdef MINMAP_BAR_HUD
		float len					= m_noise_bar.GetRange_max()-m_noise_bar.GetRange_min();
		float cur_pos				= m_luminosity_bar.GetProgressPos();
#endif
	if(cur_pos!=m_luminosity){
		float _diff = _abs(m_luminosity-cur_pos);
		if(m_luminosity>cur_pos){
			cur_pos				+= _diff*Device.fTimeDelta;
		}else{
			cur_pos				-= _diff*Device.fTimeDelta;
		}
		clamp(cur_pos, 0.f, 100.f);
		m_luminosity_shape.SetPos(cur_pos/100.f);
#ifdef MINMAP_BAR_HUD
		clamp(cur_pos, m_noise_bar.GetRange_min(), m_noise_bar.GetRange_max());
		m_luminosity_bar.SetProgressPos(cur_pos);
#endif
	}
}

void SetActorVisibility		(u16 who_id, float value)
{
	if(g_pMotionIcon)
		g_pMotionIcon->SetActorVisibility(who_id, value);
}

void CUIMotionIcon::SetActorVisibility		(u16 who_id, float value)
{
	clamp(value, 0.f, 1.f);
	value		*= 100.f;
#ifdef MINMAP_BAR_HUD
	float v		= float(m_luminosity_bar.GetRange_max() - m_luminosity_bar.GetRange_min());
	value		*= v;
	value		+= m_luminosity_bar.GetRange_min();
#endif
	xr_vector<_npc_visibility>::iterator it = std::find(m_npc_visibility.begin(), 
														m_npc_visibility.end(),
														who_id);

	if(it==m_npc_visibility.end() && value!=0)
	{
		m_npc_visibility.resize	(m_npc_visibility.size()+1);
		_npc_visibility& v		= m_npc_visibility.back();
		v.id					= who_id;
		v.value					= value;
	}
	else if( fis_zero(value) )
	{
		if (it!=m_npc_visibility.end())
			m_npc_visibility.erase(it);
	}
	else
	{
		(*it).value	= value;
	}

	m_bchanged = true;
}
