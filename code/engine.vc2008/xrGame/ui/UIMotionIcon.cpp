#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UIMotionIcon.h"
#include "../xrUICore/UIXmlInit.h"
#include "../actor.h"
const LPCSTR MOTION_ICON_XML = "ui_HUD.xml";

GAME_API CUIMotionIcon* g_pMotionIcon = nullptr;

CUIMotionIcon::CUIMotionIcon()
{
	g_pMotionIcon	= this;
	m_bchanged		= false;
	m_luminosity	= 0.0f;
	cur_pos			= 0.f;
    bVisible        = true;
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

	AttachChild					(&m_noise_shape);
	xml_init.InitProgressShape	(uiXml, "noise_shape", 0, &m_noise_shape);
	m_noise_shape.SetWndSize	(sz);
	m_noise_shape.SetWndPos	(pos);
}
void CUIMotionIcon::SetNoise(float Pos)
{
	Pos	= clampr(Pos, 0.f, 100.f);
	m_noise_shape.SetPos(Pos/100.f);
}

void CUIMotionIcon::SetLuminosity(float Pos)
{
	m_luminosity	= Pos;
}

void CUIMotionIcon::Draw()
{
    if (bVisible && !psActorFlags.test(AF_HARDCORE))
    {
        inherited::Draw();
    }
}

void CUIMotionIcon::Update()
{
if(!psActorFlags.test(AF_HARDCORE))
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
	if(cur_pos!=m_luminosity){
		float _diff = _abs(m_luminosity-cur_pos);
		if(m_luminosity>cur_pos){
			cur_pos				+= _diff*Device.fTimeDelta;
		}else{
			cur_pos				-= _diff*Device.fTimeDelta;
		}
		clamp(cur_pos, 0.f, 100.f);
		m_luminosity_shape.SetPos(cur_pos/100.f);
	}
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
