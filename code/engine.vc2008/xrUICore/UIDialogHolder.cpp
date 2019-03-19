#include "stdafx.h"
#include "UIDialogHolder.h"
#include "UIDialogWnd.h"
#include "IGameUI.h"
#include "UICursor.h"
#include "../xrEngine/IGame_Level.h"
#include "../xrEngine/IGame_Actor.h"
#include "..\xrEngine\xr_level_controller.h"
#include "../xrEngine/CustomHud.h"

dlgItem::dlgItem(CUIWindow* pWnd)
{
	wnd = pWnd;
	enabled = true;
}

bool dlgItem::operator < (const dlgItem& itm) const
{
	return (int)enabled > (int)itm.enabled;
}

bool operator == (const dlgItem& i1, const dlgItem& i2)
{
	return (i1.wnd == i2.wnd) && (i1.enabled == i2.enabled);
}

recvItem::recvItem(CUIDialogWnd* r)
{
	m_item = r;
	m_flags.zero();
}
bool operator == (const recvItem& i1, const recvItem& i2)
{
	return i1.m_item == i2.m_item;
}

CDialogHolder::CDialogHolder()
{
	m_b_in_update = false;
}

CDialogHolder::~CDialogHolder()
{
}

void CDialogHolder::StartMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	R_ASSERT(!pDialog->IsShown());

	AddDialogToRender(pDialog);
	SetMainInputReceiver(pDialog, false);

	if (UseIndicators())
	{
		bool b = !!psHUD_Flags.test(HUD_CROSSHAIR_RT);
		m_input_receivers.back().m_flags.set(recvItem::eCrosshair, b);

		b = pUIHud->GameIndicatorsShown();
		m_input_receivers.back().m_flags.set(recvItem::eIndicators, b);

		if (bDoHideIndicators)
		{
			psHUD_Flags.set(HUD_CROSSHAIR_RT, false);
			pUIHud->ShowGameIndicators(false);
		}
	}
	pDialog->SetHolder(this);

	if (pDialog->NeedCursor())
		GetUICursor().Show();

	if (g_pGameLevel)
	{
		if (g_actor && pDialog->StopAnyMove())
		{
			g_actor->StopAnyMove();
		};

		if (g_actor)
		{
			g_actor->IR_OnKeyboardRelease(kWPN_ZOOM);
			g_actor->IR_OnKeyboardRelease(kWPN_FIRE);
		}
	}
}

void CDialogHolder::StopMenu(CUIDialogWnd* pDialog)
{
	R_ASSERT(pDialog->IsShown());

	if (TopInputReceiver() == pDialog)
	{
		if (UseIndicators())
		{
			bool b = !!m_input_receivers.back().m_flags.test(recvItem::eCrosshair);
			psHUD_Flags.set(HUD_CROSSHAIR_RT, b);
			b = !!m_input_receivers.back().m_flags.test(recvItem::eIndicators);
			pUIHud->ShowGameIndicators(b);
		}

		SetMainInputReceiver(nullptr, false);
	}
	else
		SetMainInputReceiver(pDialog, true);

	RemoveDialogToRender(pDialog);
	pDialog->SetHolder(nullptr);

	if (!TopInputReceiver() || !TopInputReceiver()->NeedCursor())
		GetUICursor().Hide();
}

void CDialogHolder::AddDialogToRender(CUIWindow* pDialog)
{
	dlgItem itm(pDialog);
	itm.enabled = true;

	bool bAdd = (m_dialogsToRender_new.end() == std::find(m_dialogsToRender_new.begin(), m_dialogsToRender_new.end(), itm));

	if (!bAdd)
		return;

	bAdd = (m_dialogsToRender.end() == std::find(m_dialogsToRender.begin(), m_dialogsToRender.end(), itm));

	if (!bAdd)
		return;

	if (m_b_in_update)
		m_dialogsToRender_new.push_back(itm);
	else
		m_dialogsToRender.push_back(itm);

	pDialog->Show(true);
}

void CDialogHolder::RemoveDialogToRender(CUIWindow* pDialog)
{
	dlgItem itm(pDialog);
	itm.enabled = true;
	xr_vector<dlgItem>::iterator it = std::find(m_dialogsToRender.begin(), m_dialogsToRender.end(), itm);

	if (it != m_dialogsToRender.end())
	{
		(*it).wnd->Show(false);
		(*it).wnd->Enable(false);
		(*it).enabled = false;
	}
}

void CDialogHolder::DoRenderDialogs()
{
	xr_vector<dlgItem>::iterator it = m_dialogsToRender.begin();
	for (; it != m_dialogsToRender.end(); ++it) {
		if ((*it).enabled && (*it).wnd->IsShown())
			(*it).wnd->Draw();
	}
}

void  CDialogHolder::OnExternalHideIndicators()
{
	xr_vector<recvItem>::iterator it = m_input_receivers.begin();
	xr_vector<recvItem>::iterator it_e = m_input_receivers.end();
	for (; it != it_e; ++it)
	{
		(*it).m_flags.set(recvItem::eIndicators, false);
		(*it).m_flags.set(recvItem::eCrosshair, false);
	}
}

CUIDialogWnd* CDialogHolder::TopInputReceiver()
{
	if (!m_input_receivers.empty())
		return m_input_receivers.back().m_item;

	return nullptr;
}

void CDialogHolder::SetMainInputReceiver(CUIDialogWnd* ir, bool _find_remove)
{
	if (TopInputReceiver() == ir)
		return;

	if (!ir || _find_remove)
	{
		if (m_input_receivers.empty())
			return;

		if (ir)
		{
			VERIFY(ir && _find_remove);

			for (size_t cnt = m_input_receivers.size(); cnt > 0; --cnt)
			{
				if (m_input_receivers[cnt - 1].m_item == ir)
				{
					m_input_receivers[cnt].m_flags.set(recvItem::eCrosshair, m_input_receivers[cnt - 1].m_flags.test(recvItem::eCrosshair));
					m_input_receivers[cnt].m_flags.set(recvItem::eIndicators, m_input_receivers[cnt - 1].m_flags.test(recvItem::eIndicators));
					xr_vector<recvItem>::iterator it = m_input_receivers.begin();
					std::advance(it, cnt - 1);
					m_input_receivers.erase(it);
					break;
				}
			}
		}
		else m_input_receivers.pop_back();
	}
	else
	{
		m_input_receivers.emplace_back(ir);
	}
};

void CDialogHolder::StartDialog(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	if (pDialog)
	{
		if (pDialog->NeedCenterCursor())
			GetUICursor().SetUICursorPosition(Fvector2().set(512.0f, 384.0f));

		StartMenu(pDialog, bDoHideIndicators);
	}
}

void CDialogHolder::StopDialog(CUIDialogWnd* pDialog)
{
	StopMenu(pDialog);
}

void CDialogHolder::OnFrame()
{
	m_b_in_update = true;
	CUIDialogWnd* wnd = TopInputReceiver();
	if (wnd && wnd->IsEnabled())
	{
		wnd->Update();
	}
	//else
	{
		xr_vector<dlgItem>::iterator it = m_dialogsToRender.begin();
		for (; it != m_dialogsToRender.end(); ++it)
			if ((*it).enabled && (*it).wnd->IsEnabled())
				(*it).wnd->Update();
	}

	m_b_in_update = false;
	if (!m_dialogsToRender_new.empty())
	{
		m_dialogsToRender.insert(m_dialogsToRender.end(), m_dialogsToRender_new.begin(), m_dialogsToRender_new.end());
		m_dialogsToRender_new.clear();
	}

	std::sort(m_dialogsToRender.begin(), m_dialogsToRender.end());
	while (!m_dialogsToRender.empty() && (!m_dialogsToRender[m_dialogsToRender.size() - 1].enabled))
		m_dialogsToRender.pop_back();
}

void CDialogHolder::CleanInternals()
{
	while (!m_input_receivers.empty())
		m_input_receivers.pop_back();

	m_dialogsToRender.clear();
	GetUICursor().Hide();
}

bool CDialogHolder::IR_UIOnKeyboardPress(u8 dik)
{
	CUIDialogWnd *TIR = TopInputReceiver();

	if (!TIR)
		return false;

	if (!TIR->IR_process())
		return false;
	//mouse click
	if (isMouseButton(dik))
	{
		Fvector2 cp = GetUICursor().GetCursorPosition();
		EUIMessages action = (dik == VK_LBUTTON) ? WINDOW_LBUTTON_DOWN : (dik == VK_RBUTTON) ? WINDOW_RBUTTON_DOWN : WINDOW_CBUTTON_DOWN;
		if (TIR->OnMouseAction(cp.x, cp.y, action))
			return true;
	}

	if (TIR->OnKeyboardAction(dik, WINDOW_KEY_PRESSED))
		return true;

	if (!TIR->StopAnyMove() && g_pGameLevel)
	{
		CObject* O = g_pGameLevel->CurrentEntity();
		if (O)
		{
			IInputReceiver* IR = dynamic_cast<IInputReceiver*>(O);
			if (IR)
			{
				EGameActions action = get_binded_action(dik);
				if (action != kQUICK_USE_1 && action != kQUICK_USE_2 && action != kQUICK_USE_3 && action != kQUICK_USE_4)
					IR->IR_OnKeyboardPress(action);
			}
			return (false);
		}
	}
	return true;
}

bool CDialogHolder::IR_UIOnKeyboardRelease(u8 vKey)
{
    /// We must disable that here, because of two reasons:
    /// 1. Keyboard releases not working when actor speaking. That's means, if we enter to pickup mode, and release the kUSE button, we still be in pickup mode, because
    /// Actor doesn't know about that
    /// 2. Pickup mode is more HUD functionality
    EGameActions GameAction = get_binded_action(vKey);

	CUIDialogWnd *TIR = TopInputReceiver();

	if (!TIR)
		return false;

	if (!TIR->IR_process())
		return false;

	//mouse click
	if (isMouseButton(vKey))
	{
		Fvector2 cp = GetUICursor().GetCursorPosition();
		EUIMessages action = (vKey == VK_LBUTTON) ? WINDOW_LBUTTON_UP : (vKey == VK_RBUTTON) ? WINDOW_RBUTTON_UP : WINDOW_CBUTTON_UP;
		if (TIR->OnMouseAction(cp.x, cp.y, action))
			return true;
	}

	if (TIR->OnKeyboardAction(vKey, WINDOW_KEY_RELEASED))
		return true;

	if (!TIR->StopAnyMove() && g_pGameLevel)
	{
		CObject* O = g_pGameLevel->CurrentEntity();
		if (O)
		{
			IInputReceiver*		IR = dynamic_cast<IInputReceiver*>(O);

			if (IR)
				IR->IR_OnKeyboardRelease(GameAction);

			return (false);
		}
	}
	return true;
}

bool CDialogHolder::IR_UIOnKeyboardHold(u8 dik)
{
	CUIDialogWnd* TIR = TopInputReceiver();

	if (!TIR)
		return false;

	if (!TIR->IR_process())
		return false;

	if (TIR->OnKeyboardHold(dik))
		return true;

	if (!TIR->StopAnyMove() && g_pGameLevel)
	{
		CObject* O = g_pGameLevel->CurrentEntity();
		if (O)
		{
			IInputReceiver*	IR = dynamic_cast<IInputReceiver*>(O);

			if (IR)
				IR->IR_OnKeyboardHold(get_binded_action(dik));

			return false;
		}
	}
	return true;
}

bool CDialogHolder::IR_UIOnMouseWheel(int direction)
{
	CUIDialogWnd *TIR = TopInputReceiver();

	if (!TIR)
		return false;

	if (!TIR->IR_process())
		return false;

	Fvector2 pos = GetUICursor().GetCursorPosition();

	TIR->OnMouseAction(pos.x, pos.y, (direction > 0) ? WINDOW_MOUSE_WHEEL_UP : WINDOW_MOUSE_WHEEL_DOWN);
	return true;
}

bool CDialogHolder::IR_UIOnMouseMove(int dx, int dy)
{
	CUIDialogWnd *TIR = TopInputReceiver();

	if (!TIR)
		return false;

	if (!TIR->IR_process())
		return false;

	if (GetUICursor().IsVisible())
	{
		GetUICursor().UpdateCursorPosition(dx, dy);
		Fvector2 cPos = GetUICursor().GetCursorPosition();
		TIR->OnMouseAction(cPos.x, cPos.y, WINDOW_MOUSE_MOVE);
	}
	else
		if (!TIR->StopAnyMove() && g_pGameLevel)
		{
			CObject* O = g_pGameLevel->CurrentEntity();
			if (O)
			{
				IInputReceiver*	IR = dynamic_cast<IInputReceiver*>(O);

				if (IR)
					IR->IR_OnMouseMove(dx, dy);

				return false;
			}
		};
	return true;
}