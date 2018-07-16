#include "stdafx.h"
#include "UIGame.h"
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
#include "ui/UITalkWnd.h"
#include "actor.h"
#include "actorcondition.h"
#include "inventory.h"
#include "game_cl_base.h"

#include "GameTaskManager.h"
#include "GameTask.h"

#include "level_changer.h"

#include "xr_level_controller.h"
#include "../xrEngine/xr_input.h"
#include "../xrEngine/xr_ioconsole.h"
#include "../xrEngine/x_ray.h"
#include "../../xrServerEntities/script_engine.h"

#include "attachable_item.h"

EGameIDs ParseStringToGameType(LPCSTR str);

struct predicate_find_stat
{
	LPCSTR	m_id;
	predicate_find_stat(LPCSTR id) :m_id(id) {}
	bool operator() (SDrawStaticStruct* s)
	{
		return (s->m_name == m_id);
	}
};

CUIGame::CUIGame() : m_msgs_xml(nullptr), m_ActorMenu(nullptr), m_PdaMenu(nullptr), m_window(nullptr), UIMainIngameWnd(nullptr), m_pMessagesWnd(nullptr), m_game_objective(nullptr)
{
	ShowGameIndicators(true);
	ShowCrosshair(true);

	TalkMenu = xr_new<CUITalkWnd>();
	m_game = xr_new<game_cl_GameState>();
	UIChangeLevelWnd = xr_new<CChangeLevelWnd>();
}

bool g_b_ClearGameCaptions = false;

CUIGame::~CUIGame()
{
	delete_data(m_custom_statics);
	g_b_ClearGameCaptions = false;

	delete_data(TalkMenu);
	delete_data(UIChangeLevelWnd);
}

void CUIGame::OnFrame()
{
	CDialogHolder::OnFrame();

	for (SDrawStaticStruct* item : m_custom_statics)
		item->Update();

	auto comparer = [](const SDrawStaticStruct* s1, const SDrawStaticStruct* s2)
	{
		return s1->IsActual() > s2->IsActual();
	};

	std::sort(m_custom_statics.begin(), m_custom_statics.end(), comparer);

	while (!m_custom_statics.empty() && !m_custom_statics.back()->IsActual())
	{
		delete_data(m_custom_statics.back());
		m_custom_statics.pop_back();
	}

	if (g_b_ClearGameCaptions)
	{
		delete_data(m_custom_statics);
		g_b_ClearGameCaptions = false;
	}

	m_window->Update();

	//update windows
	if (GameIndicatorsShown() && psHUD_Flags.is(HUD_DRAW | HUD_DRAW_RT))
		UIMainIngameWnd->Update();

	m_pMessagesWnd->Update();

	if (Device.Paused())
		return;

	if (m_game_objective)
	{
		bool b_remove = false;
		int dik = get_action_dik(kSCORES, 0);

		if (dik && !pInput->iGetAsyncKeyState(dik))
			b_remove = true;

		dik = get_action_dik(kSCORES, 1);

		if (!b_remove && dik && !pInput->iGetAsyncKeyState(dik))
			b_remove = true;

		if (b_remove)
		{
			RemoveCustomStatic("main_task");
			RemoveCustomStatic("secondary_task");
			m_game_objective = nullptr;
		}
	}
}

void attach_adjust_mode_keyb(int dik);
void attach_draw_adjust_mode();
void hud_adjust_mode_keyb(int dik);
void hud_draw_adjust_mode();

void CUIGame::Render()
{
	st_vec_it it = m_custom_statics.begin();
	st_vec_it it_e = m_custom_statics.end();
	for (; it != it_e; ++it)
		(*it)->Draw();

	m_window->Draw();

	CEntity* pEntity = smart_cast<CEntity*>(Level().CurrentEntity());
	if (pEntity)
	{
		CActor* pActor = smart_cast<CActor*>(pEntity);
		if (pActor && pActor->HUDview() && pActor->g_Alive() && psHUD_Flags.is(HUD_WEAPON | HUD_WEAPON_RT | HUD_WEAPON_RT2))
		{
			u16 ISlot = pActor->inventory().FirstSlot();
			u16 ESlot = pActor->inventory().LastSlot();

			for (; ISlot <= ESlot; ++ISlot)
			{
				PIItem itm = pActor->inventory().ItemFromSlot(ISlot);
				if (itm && itm->render_item_ui_query())
					itm->render_item_ui();
			}
		}

		if (GameIndicatorsShown() && psHUD_Flags.is(HUD_DRAW | HUD_DRAW_RT))
			UIMainIngameWnd->Draw();
	}

	m_pMessagesWnd->Draw();

	DoRenderDialogs();

	hud_draw_adjust_mode();
	attach_draw_adjust_mode();
}

SDrawStaticStruct* CUIGame::AddCustomStatic(LPCSTR id, bool bSingleInstance)
{
	if (bSingleInstance)
	{
		st_vec::iterator it = std::find_if(m_custom_statics.begin(), m_custom_statics.end(), predicate_find_stat(id));
		if (it != m_custom_statics.end())
			return (*it);
	}

	CUIXmlInit xml_init;
	m_custom_statics.push_back(xr_new<SDrawStaticStruct>());
	SDrawStaticStruct* sss = m_custom_statics.back();

	sss->m_static = xr_new<CUIStatic>();
	sss->m_name = id;
	xml_init.InitStatic(*m_msgs_xml, id, 0, sss->m_static);
	float ttl = m_msgs_xml->ReadAttribFlt(id, 0, "ttl", -1);
	if (ttl > 0.0f)
		sss->m_endTime = Device.fTimeGlobal + ttl;

	return sss;
}

SDrawStaticStruct* CUIGame::GetCustomStatic(LPCSTR id)
{
	st_vec::iterator it = std::find_if(m_custom_statics.begin(), m_custom_statics.end(), predicate_find_stat(id));
	if (it != m_custom_statics.end())
		return (*it);

	return nullptr;
}

void CUIGame::RemoveCustomStatic(LPCSTR id)
{
	st_vec::iterator it = std::find_if(m_custom_statics.begin(), m_custom_statics.end(), predicate_find_stat(id));
	if (it != m_custom_statics.end())
	{
		delete_data(*it);
		m_custom_statics.erase(it);
	}
}

void CUIGame::OnInventoryAction(PIItem item, u16 action_type)
{
	if (m_ActorMenu->IsShown())
		m_ActorMenu->OnInventoryAction(item, action_type);
}

#include "ui/UIGameTutorial.h"

extern CUISequencer* g_tutorial;
extern CUISequencer* g_tutorial2;

bool CUIGame::ShowActorMenu()
{
	if (m_ActorMenu->IsShown())
	{
		m_ActorMenu->HideDialog();
	}
	else
	{
		HidePdaMenu();
		CInventoryOwner* pIOActor = smart_cast<CInventoryOwner*>(Level().CurrentViewEntity());
		VERIFY(pIOActor);
		m_ActorMenu->SetActor(pIOActor);
		m_ActorMenu->SetMenuMode(mmInventory);
		m_ActorMenu->ShowDialog(true);
	}
	return true;
}

void CUIGame::HideActorMenu()
{
	if (m_ActorMenu->IsShown())
		m_ActorMenu->HideDialog();
}

void CUIGame::HideMessagesWindow()
{
	if (m_pMessagesWnd->IsShown())
		m_pMessagesWnd->Show(false);
}

void CUIGame::ShowMessagesWindow()
{
	if (!m_pMessagesWnd->IsShown())
		m_pMessagesWnd->Show(true);
}

bool CUIGame::ShowPdaMenu()
{
	HideActorMenu();
	m_PdaMenu->ShowDialog(true);
	return true;
}

void CUIGame::HidePdaMenu()
{
	if (m_PdaMenu->IsShown())
		m_PdaMenu->HideDialog();
}

void CUIGame::SetClGame(game_cl_GameState* g)
{
	g->SetGameUI(this);
	m_game = smart_cast<game_cl_GameState*>(g);
	R_ASSERT(m_game);
}

void CUIGame::UnLoad()
{
	xr_delete(m_msgs_xml);
	xr_delete(m_ActorMenu);
	xr_delete(m_PdaMenu);
	xr_delete(m_window);
	xr_delete(UIMainIngameWnd);
	xr_delete(m_pMessagesWnd);
}

void CUIGame::Load()
{
	if (g_pGameLevel)
	{
		R_ASSERT(!m_msgs_xml);
		m_msgs_xml = xr_new<CUIXml>();
		m_msgs_xml->Load(CONFIG_PATH, UI_PATH, "ui_custom_msgs.xml");

		R_ASSERT(!m_ActorMenu);
		m_ActorMenu = xr_new<CUIActorMenu>();

		R_ASSERT(!m_PdaMenu);
		m_PdaMenu = xr_new<CUIPdaWnd>();

		R_ASSERT(!m_window);
		m_window = xr_new<CUIWindow>();

		R_ASSERT(!UIMainIngameWnd);
		UIMainIngameWnd = xr_new<CUIMainIngameWnd>();
		UIMainIngameWnd->Init();

		R_ASSERT(!m_pMessagesWnd);
		m_pMessagesWnd = xr_new<CUIMessagesWindow>();

		Init(0);
		Init(1);
		Init(2);
	}
}

void CUIGame::OnConnected()
{
	if (g_pGameLevel)
	{
		if (!UIMainIngameWnd)
			Load();

		UIMainIngameWnd->OnConnected();
	}
}

void CUIGame::CommonMessageOut(LPCSTR text)
{
	m_pMessagesWnd->AddLogMessage(text);
}

void CUIGame::UpdatePda()
{
	PdaMenu().UpdatePda();
}

void CUIGame::update_fake_indicators(u8 type, float power)
{
	UIMainIngameWnd->get_hud_states()->FakeUpdateIndicatorType(type, power);
}

void CUIGame::enable_fake_indicators(bool enable)
{
	UIMainIngameWnd->get_hud_states()->EnableFakeIndicators(enable);
}

void CUIGame::HideShownDialogs()
{
	HideActorMenu();
	HidePdaMenu();
	CUIDialogWnd* mir = TopInputReceiver();

	if (mir && mir == TalkMenu)
		mir->HideDialog();
}

bool CUIGame::IR_UIOnKeyboardPress(int dik)
{
	if (CDialogHolder::IR_UIOnKeyboardPress(dik))
		return true;

	if (Device.Paused())
		return false;

	hud_adjust_mode_keyb(dik);
	attach_adjust_mode_keyb(dik);

	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	if (!pInvOwner)
		return false;

	CEntityAlive* EA = smart_cast<CEntityAlive*>(Level().CurrentEntity());
	if (!EA || !EA->g_Alive())
		return false;

	CActor *pActor = smart_cast<CActor*>(pInvOwner);
	if (!pActor)
		return false;

	if (!pActor->g_Alive())
		return false;

	switch (get_binded_action(dik))
	{
		case kACTIVE_JOBS:
		{
			if (!pActor->inventory_disabled() && !Actor()->HasInfo("pda_has_blocked"))
			{
				luabind::functor<void> functor;
				if (ai().script_engine().functor("oxy_callbacks.Pda_Activate", functor))
					functor();

				ShowPdaMenu();
			}

			break;
		}

		case kINVENTORY:
		{
			if (!pActor->inventory_disabled())
				ShowActorMenu();

			break;
		}

		case kSCORES:
			if (!pActor->inventory_disabled())
			{
				m_game_objective = AddCustomStatic("main_task", true);
				CGameTask* t1 = Level().GameTaskManager().ActiveTask();
				m_game_objective->m_static->TextItemControl()->SetTextST((t1) ? t1->m_Title.c_str() : "st_no_active_task");

				if (t1 && t1->m_Description.c_str())
				{
					SDrawStaticStruct* sm2 = AddCustomStatic("secondary_task", true);
					sm2->m_static->TextItemControl()->SetTextST(t1->m_Description.c_str());
				}
			}
			break;
	}

	return false;
}

void CUIGame::StartTrade(CInventoryOwner* pActorInv, CInventoryOwner* pOtherOwner)
{
	m_ActorMenu->SetActor(pActorInv);
	m_ActorMenu->SetPartner(pOtherOwner);

	m_ActorMenu->SetMenuMode(mmTrade);
	m_ActorMenu->ShowDialog(true);
}

void CUIGame::StartUpgrade(CInventoryOwner* pActorInv, CInventoryOwner* pMech)
{
	m_ActorMenu->SetActor(pActorInv);
	m_ActorMenu->SetPartner(pMech);

	m_ActorMenu->SetMenuMode(mmUpgrade);
	m_ActorMenu->ShowDialog(true);
}

void CUIGame::StartTalk(bool disable_break)
{
	RemoveCustomStatic("main_task");
	RemoveCustomStatic("secondary_task");

	TalkMenu->b_disable_break = disable_break;
	TalkMenu->ShowDialog(true);
}

void CUIGame::StartSearchBody(CInventoryOwner* pActorInv, CInventoryOwner* pOtherOwner) //Deadbody search
{
	if (TopInputReceiver())
		return;

    // Don't allow search monster's if that feature disabled
    if (!g_extraFeatures.is(GAME_EXTRA_MONSTER_INVENTORY)) return;

	m_ActorMenu->SetActor(pActorInv);
	m_ActorMenu->SetPartner(pOtherOwner);

	m_ActorMenu->SetMenuMode(mmDeadBodySearch);
	m_ActorMenu->ShowDialog(true);
}

void CUIGame::StartSearchBody(CInventoryOwner* pActorInv, CInventoryBox* pBox) //Deadbody search
{
	if (TopInputReceiver())
		return;

	m_ActorMenu->SetActor(pActorInv);
	m_ActorMenu->SetInvBox(pBox);
	VERIFY(pBox);

	m_ActorMenu->SetMenuMode(mmDeadBodySearch);
	m_ActorMenu->ShowDialog(true);
}

CUIGame* get_hud()
{
	return GameUI();
}

void CUIGame::ChangeLevel(GameGraph::_GRAPH_ID game_vert_id, u32 level_vert_id, Fvector pos, Fvector ang, Fvector pos2, Fvector ang2, bool b_use_position_cancel, const shared_str& message_str, bool b_allow_change_level)
{
	if (TopInputReceiver() != UIChangeLevelWnd)
	{
		UIChangeLevelWnd->m_game_vertex_id = game_vert_id;
		UIChangeLevelWnd->m_level_vertex_id = level_vert_id;
		UIChangeLevelWnd->m_position = pos;
		UIChangeLevelWnd->m_angles = ang;
		UIChangeLevelWnd->m_position_cancel = pos2;
		UIChangeLevelWnd->m_angles_cancel = ang2;
		UIChangeLevelWnd->m_b_position_cancel = b_use_position_cancel;
		UIChangeLevelWnd->m_b_allow_change_level = b_allow_change_level;
		UIChangeLevelWnd->m_message_str = message_str;

		UIChangeLevelWnd->ShowDialog(true);
	}
}

SDrawStaticStruct::SDrawStaticStruct()
{
	m_static = nullptr;
	m_endTime = -1.0f;
}

void SDrawStaticStruct::destroy()
{
	delete_data(m_static);
}

bool SDrawStaticStruct::IsActual() const
{
	if (m_endTime < 0)
		return true;

	return (Device.fTimeGlobal < m_endTime);
}

void SDrawStaticStruct::SetText(LPCSTR text)
{
	m_static->Show(text != nullptr);
	if (text)
	{
		m_static->TextItemControl()->SetTextST(text);
		m_static->ResetColorAnimation();
	}
}

void SDrawStaticStruct::Draw()
{
	if (m_static->IsShown())
		m_static->Draw();
}

void SDrawStaticStruct::Update()
{
	if (IsActual() && m_static->IsShown())
		m_static->Update();
}

using namespace luabind;

#pragma optimize("s",on)
void CUIGame::script_register(lua_State *L)
{
	module(L)
		[
			class_< SDrawStaticStruct >("SDrawStaticStruct")
			.def_readwrite("m_endTime", &SDrawStaticStruct::m_endTime)
		.def("wnd", &SDrawStaticStruct::wnd),

		class_<CUIGame>("CUIGame")
		.def("AddDialogToRender",		&CUIGame::AddDialogToRender)
		.def("RemoveDialogToRender",	&CUIGame::RemoveDialogToRender)
		.def("AddCustomStatic",			&CUIGame::AddCustomStatic)
		.def("RemoveCustomStatic",		&CUIGame::RemoveCustomStatic)
		.def("HideActorMenu",			&CUIGame::HideActorMenu)
		.def("HidePdaMenu",				&CUIGame::HidePdaMenu)
		.def("ShowPdaMenu",				&CUIGame::ShowPdaMenu)
		.def("show_messages",			&CUIGame::ShowMessagesWindow)
		.def("hide_messages",			&CUIGame::HideMessagesWindow)
		.def("GetCustomStatic",			&CUIGame::GetCustomStatic)
		.def("update_fake_indicators",	&CUIGame::update_fake_indicators)
		.def("enable_fake_indicators",  &CUIGame::enable_fake_indicators),
		def("get_hud", &get_hud)
		];
}
