#include "stdafx.h"
#include "HUDManager.h"
#include "hudtarget.h"
#include "actor.h"
#include "../xrEngine/igame_level.h"
#include "../xrEngine/xr_input.h"
#include "GamePersistent.h"
#include "../xrUICore/MainMenu.h"
#include "items/Grenade.h"
#include "Car.h"
#include "UIGame.h"
#include "../xrUICore/UICursor.h"
#include "../xrEngine/string_table.h"
#include "../xrEngine/IGame_AnselSDK.h"
#ifdef	DEBUG
#include "phdebug.h"
#endif

extern CUIGame* GameUI() { return g_hud ? HUD().GetGameUI() : nullptr; }

CHUDManager::CHUDManager() : m_pHUDTarget(xr_new<CHUDTarget>()) 
{
	pUIHud = new CUIGame();
}

CHUDManager::~CHUDManager()
{
	OnDisconnected();

	if (pUIHud)
	{
		pUIHud->UnLoad();
		xr_delete(pUIHud);
	}

	xr_delete(m_pHUDTarget);
}

void CHUDManager::OnFrame()
{
	if (!psHUD_Flags.is(HUD_DRAW_RT2) || !b_online)
		return;

	if (pUIHud)
		GetGameUI()->OnFrame();

	m_pHUDTarget->CursorOnFrame();
}

ENGINE_API extern float psHUD_FOV;

void CHUDManager::Render_First()
{
	if (!psHUD_Flags.is(HUD_WEAPON | HUD_WEAPON_RT | HUD_WEAPON_RT2 | HUD_DRAW_RT2))
		return;

	if (!pUIHud)
		return;

	CObject *O = g_pGameLevel->CurrentViewEntity();
	if (!O)
		return;

	CActor* A = smart_cast<CActor*> (O);

	if (!A || !A->HUDview())
		return;

	// only shadow
	::Render->set_Invisible(TRUE);
	::Render->set_Object(O->H_Root());
	O->renderable_Render();
	::Render->set_Invisible(FALSE);
}

bool need_render_hud()
{
	if (!IGameAnsel::IsActive())
	{
		if (g_pGameLevel && g_pGameLevel->CurrentViewEntity())
		{
			CActor* A = smart_cast<CActor*> (g_pGameLevel->CurrentViewEntity());
			if (A && (!A->HUDview() || !A->g_Alive()))
				return false;

			if (!smart_cast<CCar*>(g_pGameLevel->CurrentViewEntity()))
				return true;
		}
	}
	return false;
}

void CHUDManager::Render_Last()
{
	if (!psHUD_Flags.is(HUD_WEAPON | HUD_WEAPON_RT | HUD_WEAPON_RT2 | HUD_DRAW_RT2))
		return;

	if (!pUIHud || !need_render_hud())
		return;

	CObject *O = g_pGameLevel->CurrentViewEntity();
	// hud itself
	::Render->set_HUD(TRUE);
	::Render->set_Object(O->H_Root());
	O->OnHUDDraw(this);
	::Render->set_HUD(FALSE);
}

void CHUDManager::Render_Actor_Shadow() // added by KD
{
	if (0 == pUIHud)
		return;

	CObject* O = g_pGameLevel->CurrentViewEntity();
	if (0 == O)
		return;

	CActor* A = smart_cast<CActor*> (O);
	if (!A)
		return;

	if (A->active_cam() != eacFirstEye) // KD: we need to render actor shadow only in first eye cam mode because
		return;// in other modes actor model already in scene graph and renders well

	::Render->set_Object(O->H_Root());
	O->renderable_Render();
}

#include "player_hud.h"
bool   CHUDManager::RenderActiveItemUIQuery()
{
	if (!psHUD_Flags.is(HUD_DRAW_RT2))
		return false;

	if (!psHUD_Flags.is(HUD_WEAPON | HUD_WEAPON_RT | HUD_WEAPON_RT2))return false;

	if (!need_render_hud())			return false;

	return (g_player_hud && g_player_hud->render_item_ui_query());
}

void   CHUDManager::RenderActiveItemUI()
{
	if (!psHUD_Flags.is(HUD_DRAW_RT2))
		return;

	g_player_hud->render_item_ui();
}

extern ENGINE_API BOOL bShowPauseString;
//отрисовка элементов интерфейса
void  CHUDManager::RenderUI()
{
	if (!psHUD_Flags.is(HUD_DRAW_RT2)) return;

	if (!b_online) return;

	HitMarker.Render();
	if (pUIHud) GetGameUI()->Render();

	UI().RenderFont();
	m_pHUDTarget->Render();

	if (Device.Paused() && bShowPauseString)
	{
		CGameFont* pFont = UI().Font().GetFont("ui_font_graff_50");
		pFont->SetColor(0x80FF0000);
		LPCSTR _str = CStringTable().translate("st_game_paused").c_str();

		Fvector2 _pos;
		_pos.set(UI_BASE_WIDTH / 2.0f, UI_BASE_HEIGHT / 2.0f);
		UI().ClientToScreenScaled(_pos);
		pFont->SetAligment(CGameFont::alCenter);
		pFont->Out(_pos.x, _pos.y, _str);
		pFont->OnRender();
	}

	if (psActorFlags.test(AF_WORKINPROGRESS))
	{
		CGameFont* pFont = UI().Font().GetFont("ui_font_graffiti19_russian");
		pFont->SetColor(D3DCOLOR_XRGB(216, 216, 216));
		LPCSTR _str = CStringTable().translate("Work In Progress").c_str();

		Fvector2			_pos;
		_pos.set(20, 650);
		UI().ClientToScreenScaled(_pos);
		pFont->SetAligment(CGameFont::alLeft);
		pFont->Out(_pos.x, _pos.y, _str);
		pFont->OnRender();
	}
}

void CHUDManager::OnEvent(EVENT E, u64 P1, u64 P2)
{
}

collide::rq_result&	CHUDManager::GetCurrentRayQuery()
{
	return m_pHUDTarget->GetRQ();
}

void CHUDManager::SetCrosshairDisp(float dispf, float disps)
{
	m_pHUDTarget->GetHUDCrosshair().SetDispersion(psHUD_Flags.test(HUD_CROSSHAIR_DYNAMIC) ? dispf : disps);
}

#ifdef DEBUG
void CHUDManager::SetFirstBulletCrosshairDisp(float fbdispf)
{
	m_pHUDTarget->GetHUDCrosshair().SetFirstBulletDispertion(fbdispf);
}
#endif

void  CHUDManager::ShowCrosshair(bool show)
{
	m_pHUDTarget->ShowCrosshair(show);
}

void CHUDManager::HitMarked(int idx, float power, const Fvector& dir)
{
	HitMarker.Hit(dir);
	power *= 2.0f;
	clamp(power, 0.0f, 1.0f);
	pInput->feedback(u16(iFloor(u16(-1)*power)), u16(iFloor(u16(-1)*power)), 0.5f);
}

bool CHUDManager::AddGrenade_ForMark(CGrenade* grn)
{
	return HitMarker.AddGrenade_ForMark(grn);
}

void CHUDManager::Update_GrenadeView(Fvector& pos_actor)
{
	HitMarker.Update_GrenadeView(pos_actor);
}

void CHUDManager::SetHitmarkType(LPCSTR tex_name)
{
	HitMarker.InitShader(tex_name);
}

void CHUDManager::SetGrenadeMarkType(LPCSTR tex_name)
{
	HitMarker.InitShader_Grenade(tex_name);
}

// ------------------------------------------------------------------------------------

#include "ui\UIMainInGameWnd.h"
extern CUIXml* pWpnScopeXml;

void CHUDManager::Load()
{
	if (!pUIHud)
	{
		pUIHud = new CUIGame();
		GetGameUI()->Load();
		GetGameUI()->Init(0);
		GetGameUI()->Init(1);
		GetGameUI()->Init(2);
	}
}

void CHUDManager::OnScreenResolutionChanged()
{
	GetGameUI()->HideShownDialogs();

	xr_delete(pWpnScopeXml);

	GetGameUI()->UnLoad();
	GetGameUI()->Load();

	GetGameUI()->OnConnected();
}

void CHUDManager::OnDisconnected()
{
	b_online = false;
	if (pUIHud)
		Device.seqFrame.Remove(GetGameUI());
}

void CHUDManager::OnConnected()
{
	if (b_online)
		return;

	b_online = true;

	if (pUIHud)
		Device.seqFrame.Add(GetGameUI(), REG_PRIORITY_LOW - 1000);
}

void CHUDManager::net_Relcase(CObject* obj)
{
	HitMarker.net_Relcase(obj);

	VERIFY(m_pHUDTarget);
	m_pHUDTarget->net_Relcase(obj);

#ifdef	DEBUG
	DBG_PH_NetRelcase(obj);
#endif
}