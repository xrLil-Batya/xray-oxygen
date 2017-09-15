#include "stdafx.h"
#include "UIGameCustom.h"
#include "level.h"
#include "ui/UIXmlInit.h"
#include "ui/UIStatic.h"
#include "object_broker.h"
#include "string_table.h"

#include "InventoryOwner.h"
#include "ui/UIActorMenu.h"
#include "ui/UIPdaWnd.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIMessagesWindow.h"
#include "ui/UIHudStatesWnd.h"
#include "actor.h"
#include "inventory.h"
#include "game_cl_base.h"

#include "../xrEngine/x_ray.h"

EGameIDs ParseStringToGameType(LPCSTR str);

struct predicate_find_stat 
{
	LPCSTR	m_id;
	predicate_find_stat(LPCSTR id):m_id(id)	{}
	bool	operator() (SDrawStaticStruct* s) 
	{
		return ( s->m_name==m_id );
	}
};

CUIGameCustom::CUIGameCustom()
:m_msgs_xml(NULL),m_ActorMenu(NULL),m_PdaMenu(NULL),m_window(NULL),UIMainIngameWnd(NULL),m_pMessagesWnd(NULL)
{
	ShowGameIndicators		(true);
	ShowCrosshair			(true);

}
bool g_b_ClearGameCaptions = false;

CUIGameCustom::~CUIGameCustom()
{
	delete_data				(m_custom_statics);
	g_b_ClearGameCaptions	= false;
}


void CUIGameCustom::OnFrame() 
{
	CDialogHolder::OnFrame();
	
	for (SDrawStaticStruct* item : m_custom_statics)
		item->Update();
	
	auto comparer = [](const SDrawStaticStruct* s1, const SDrawStaticStruct* s2)
	{
		return s1->IsActual() > s2->IsActual();
	};
	std::sort(m_custom_statics.begin(), m_custom_statics.end(), comparer);
 
	while(!m_custom_statics.empty() && !m_custom_statics.back()->IsActual())
	{
		delete_data					(m_custom_statics.back());
		m_custom_statics.pop_back	();
	}
	
	if(g_b_ClearGameCaptions)
	{
		delete_data				(m_custom_statics);
		g_b_ClearGameCaptions	= false;
	}
	m_window->Update();

	//update windows
	if( GameIndicatorsShown() && psHUD_Flags.is(HUD_DRAW|HUD_DRAW_RT) )
		UIMainIngameWnd->Update	();

	m_pMessagesWnd->Update();
}

void CUIGameCustom::Render()
{
	st_vec_it it = m_custom_statics.begin();
	st_vec_it it_e = m_custom_statics.end();
	for(;it!=it_e;++it)
		(*it)->Draw();

	m_window->Draw();

	CEntity* pEntity = smart_cast<CEntity*>(Level().CurrentEntity());
	if (pEntity)
	{
		CActor* pActor			=	smart_cast<CActor*>(pEntity);
		if(pActor && pActor->HUDview() && pActor->g_Alive() && psHUD_Flags.is(HUD_WEAPON|HUD_WEAPON_RT|HUD_WEAPON_RT2))
		{
			u16 ISlot = pActor->inventory().FirstSlot();
			u16 ESlot = pActor->inventory().LastSlot();

			for( ; ISlot<=ESlot; ++ISlot)
			{
				PIItem itm			= pActor->inventory().ItemFromSlot(ISlot);
				if(itm && itm->render_item_ui_query())
					itm->render_item_ui();
			}
		}

		if( GameIndicatorsShown() && psHUD_Flags.is(HUD_DRAW | HUD_DRAW_RT) )
			UIMainIngameWnd->Draw();
	}

	m_pMessagesWnd->Draw();

	DoRenderDialogs();
}

SDrawStaticStruct* CUIGameCustom::AddCustomStatic(LPCSTR id, bool bSingleInstance)
{
	if(bSingleInstance)
	{
		st_vec::iterator it = std::find_if(m_custom_statics.begin(),m_custom_statics.end(), predicate_find_stat(id) );
		if(it!=m_custom_statics.end())
			return (*it);
	}
	
	CUIXmlInit xml_init;
	m_custom_statics.push_back		( xr_new<SDrawStaticStruct>() );
	SDrawStaticStruct* sss			= m_custom_statics.back();

	sss->m_static					= xr_new<CUIStatic>();
	sss->m_name						= id;
	xml_init.InitStatic				(*m_msgs_xml, id, 0, sss->m_static);
	float ttl						= m_msgs_xml->ReadAttribFlt(id, 0, "ttl", -1);
	if(ttl>0.0f)
		sss->m_endTime				= Device.fTimeGlobal + ttl;

	return sss;
}

SDrawStaticStruct* CUIGameCustom::GetCustomStatic(LPCSTR id)
{
	st_vec::iterator it = std::find_if(m_custom_statics.begin(),m_custom_statics.end(), predicate_find_stat(id));
	if(it!=m_custom_statics.end())
		return (*it);

	return NULL;
}

void CUIGameCustom::RemoveCustomStatic(LPCSTR id)
{
	st_vec::iterator it = std::find_if(m_custom_statics.begin(),m_custom_statics.end(), predicate_find_stat(id) );
	if(it!=m_custom_statics.end())
	{
			delete_data				(*it);
		m_custom_statics.erase	(it);
	}
}

void CUIGameCustom::OnInventoryAction(PIItem item, u16 action_type)
{
	if ( m_ActorMenu->IsShown() )
		m_ActorMenu->OnInventoryAction( item, action_type );
}

#include "ui/UIGameTutorial.h"

extern CUISequencer* g_tutorial;
extern CUISequencer* g_tutorial2;

bool CUIGameCustom::ShowActorMenu()
{
	if ( m_ActorMenu->IsShown() )
	{
		m_ActorMenu->HideDialog();
	}else
	{
		HidePdaMenu();
		CInventoryOwner* pIOActor	= smart_cast<CInventoryOwner*>( Level().CurrentViewEntity() );
		VERIFY						(pIOActor);
		m_ActorMenu->SetActor		(pIOActor);
		m_ActorMenu->SetMenuMode	(mmInventory);
		m_ActorMenu->ShowDialog		(true);
	}
	return true;
}

void CUIGameCustom::HideActorMenu()
{
	if ( m_ActorMenu->IsShown() )
	{
		m_ActorMenu->HideDialog();
	}
}

void CUIGameCustom::HideMessagesWindow()
{
	if ( m_pMessagesWnd->IsShown() )
		m_pMessagesWnd->Show(false);
}

void CUIGameCustom::ShowMessagesWindow()
{
	if ( !m_pMessagesWnd->IsShown() )
		m_pMessagesWnd->Show(true);
}

bool CUIGameCustom::ShowPdaMenu()
{
	HideActorMenu();
	m_PdaMenu->ShowDialog(true);
	return true;
}

void CUIGameCustom::HidePdaMenu()
{
	if ( m_PdaMenu->IsShown() )
	{
		m_PdaMenu->HideDialog();
	}
}

void CUIGameCustom::SetClGame(game_cl_GameState* g)
{
	g->SetGameUI(this);
}

void CUIGameCustom::UnLoad()
{
	xr_delete					(m_msgs_xml);
	xr_delete					(m_ActorMenu);
	xr_delete					(m_PdaMenu);
	xr_delete					(m_window);
	xr_delete					(UIMainIngameWnd);
	xr_delete					(m_pMessagesWnd);
}

void CUIGameCustom::Load()
{
	if(g_pGameLevel)
	{
		R_ASSERT				(NULL==m_msgs_xml);
		m_msgs_xml				= xr_new<CUIXml>();
		m_msgs_xml->Load		(CONFIG_PATH, UI_PATH, "ui_custom_msgs.xml");

		R_ASSERT				(NULL==m_ActorMenu);
		m_ActorMenu				= xr_new<CUIActorMenu>		();

		R_ASSERT				(NULL==m_PdaMenu);
		m_PdaMenu				= xr_new<CUIPdaWnd>			();
		
		R_ASSERT				(NULL==m_window);
		m_window				= xr_new<CUIWindow>			();

		R_ASSERT				(NULL==UIMainIngameWnd);
		UIMainIngameWnd			= xr_new<CUIMainIngameWnd>	();
		UIMainIngameWnd->Init	();

		R_ASSERT				(NULL==m_pMessagesWnd);
		m_pMessagesWnd			= xr_new<CUIMessagesWindow>();
		
		Init					(0);
		Init					(1);
		Init					(2);
	}
}

void CUIGameCustom::OnConnected()
{
	if(g_pGameLevel)
	{
		if(!UIMainIngameWnd)
			Load();

		UIMainIngameWnd->OnConnected();
	}
}

void CUIGameCustom::CommonMessageOut(LPCSTR text)
{
	m_pMessagesWnd->AddLogMessage(text);
}
void CUIGameCustom::UpdatePda()
{
	PdaMenu().UpdatePda();
}

void CUIGameCustom::update_fake_indicators(u8 type, float power)
{
	UIMainIngameWnd->get_hud_states()->FakeUpdateIndicatorType(type, power);
}

void CUIGameCustom::enable_fake_indicators(bool enable)
{
	UIMainIngameWnd->get_hud_states()->EnableFakeIndicators(enable);
}

SDrawStaticStruct::SDrawStaticStruct	()
{
	m_static	= NULL;
	m_endTime	= -1.0f;	
}

void SDrawStaticStruct::destroy()
{
	delete_data(m_static);
}

bool SDrawStaticStruct::IsActual() const
{
	if(m_endTime<0)			return true;
	return (Device.fTimeGlobal < m_endTime);
}

void SDrawStaticStruct::SetText(LPCSTR text)
{
	m_static->Show(text!=NULL);
	if(text)
	{
		m_static->TextItemControl()->SetTextST(text);
		m_static->ResetColorAnimation();
	}
}

void SDrawStaticStruct::Draw()
{
	if(m_static->IsShown())
		m_static->Draw();
}

void SDrawStaticStruct::Update()
{
	if(IsActual() && m_static->IsShown())	
		m_static->Update();
}

