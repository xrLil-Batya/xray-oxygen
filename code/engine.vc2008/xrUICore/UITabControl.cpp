#include "StdAfx.h"
#include "UITabControl.h"
#include "UITabButton.h"

CUITabControl::CUITabControl()
	 :m_cGlobalTextColor	(0xFFFFFFFF),
	  m_cActiveTextColor	(0xFFFFFFFF),
	  m_cActiveButtonColor	(0xFFFFFFFF),
	  m_cGlobalButtonColor	(0xFFFFFFFF),
	  m_bAcceleratorsEnable	(true)
{}

CUITabControl::~CUITabControl()
{
	RemoveAll();
}

void CUITabControl::SetCurrentOptValue()
{
	CUIOptionsItem::SetCurrentOptValue();
	shared_str v			= GetOptStringValue();
	CUITabButton* b			= GetButtonById(v);
	if(nullptr==b)
	{
#ifndef MASTER_GOLD
		Msg("! tab named [%s] doesnt exist", v.c_str());
#endif // #ifndef MASTER_GOLD
		v					= m_TabsArr[0]->m_btn_id;
	}
	SetActiveTab			(v);
}

void CUITabControl::SaveOptValue()
{
	CUIOptionsItem::SaveOptValue();
	SaveOptStringValue			(GetActiveId().c_str());
}

void CUITabControl::UndoOptValue()
{
	SetActiveTab		(m_opt_backup_value);
	CUIOptionsItem::UndoOptValue();
}

void CUITabControl::SaveBackUpOptValue()
{
	CUIOptionsItem::SaveBackUpOptValue();
	m_opt_backup_value	= GetActiveId();
}

bool CUITabControl::IsChangedOptValue() const
{
	return GetActiveId() != m_opt_backup_value;
}

// добавление кнопки-закладки в список закладок контрола
bool CUITabControl::AddItem(LPCSTR pItemName, LPCSTR pTexName, Fvector2 pos, Fvector2 size)
{
	CUITabButton *pNewButton = xr_new<CUITabButton>();
	pNewButton->SetAutoDelete	(true);
	pNewButton->InitButton		(pos, size);
	pNewButton->InitTexture		(pTexName);
	pNewButton->TextItemControl()->SetText(pItemName);
	pNewButton->TextItemControl()->SetTextColor	(m_cGlobalTextColor);
	pNewButton->SetTextureColor	(m_cGlobalButtonColor);

	return AddItem				(pNewButton);
}

bool CUITabControl::AddItem(CUITabButton *pButton)
{
	pButton->SetAutoDelete		(true);
	pButton->Show				(true);
	pButton->Enable				(true);
	pButton->SetButtonAsSwitch	(true);

	AttachChild					(pButton);
	m_TabsArr.push_back			(pButton);
	R_ASSERT					(pButton->m_btn_id.size());
	return						true;
}

void CUITabControl::RemoveAll()
{
    auto it = m_TabsArr.begin();
	for (; it != m_TabsArr.end(); ++it)
	{
		DetachChild(*it);
	}
	m_TabsArr.clear();
}

void CUITabControl::SendMessageToWnd(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (TAB_CHANGED == msg)
	{
		for (u32 i = 0; i < m_TabsArr.size(); ++i)
		{
			if (m_TabsArr[i] == pWnd)
			{
				m_sPushedId = m_TabsArr[i]->m_btn_id;
				if (m_sPrevPushedId == m_sPushedId)
					return;
                
				OnTabChange(m_sPushedId, m_sPrevPushedId);
				m_sPrevPushedId = m_sPushedId;							
				break;
			}
		}
	}

	else if (WINDOW_FOCUS_RECEIVED	== msg	||
			 WINDOW_FOCUS_LOST		== msg)
	{
		for (u8 i = 0; i < m_TabsArr.size(); ++i)
		{
			if (pWnd == m_TabsArr[i])
			{				
				if (msg == WINDOW_FOCUS_RECEIVED)
                    OnStaticFocusReceive(pWnd);
				else
					OnStaticFocusLost(pWnd);
			}
		}
	}
	else
	{
		inherited::SendMessageToWnd(pWnd, msg, pData);
	}
}

void CUITabControl::OnStaticFocusReceive(CUIWindow* pWnd)
{
	GetMessageTarget()->SendMessageToWnd			(this, WINDOW_FOCUS_RECEIVED, static_cast<void*>(pWnd));
}

void CUITabControl::OnStaticFocusLost(CUIWindow* pWnd)
{
	GetMessageTarget()->SendMessageToWnd			(this, WINDOW_FOCUS_LOST, static_cast<void*>(pWnd));
}

void CUITabControl::OnTabChange(const shared_str& sCur, const shared_str& sPrev)
{
	CUITabButton* tb_cur					= GetButtonById			(sCur);
	CUITabButton* tb_prev					= GetButtonById			(sPrev);
	if(tb_prev)	
		tb_prev->SendMessageToWnd				(tb_cur, TAB_CHANGED, nullptr);

	tb_cur->SendMessageToWnd						(tb_cur, TAB_CHANGED, nullptr);	

	GetMessageTarget()->SendMessageToWnd			(this, TAB_CHANGED, nullptr);
}

void CUITabControl::SetActiveTab(const shared_str& sNewTab)
{
	if (m_sPushedId == sNewTab)
		return;
    
	m_sPushedId			= sNewTab;
	OnTabChange			(m_sPushedId, m_sPrevPushedId);
	
	m_sPrevPushedId		= m_sPushedId;
}

bool CUITabControl::OnKeyboardAction(u8 dik, EUIMessages keyboard_action)
{

	if (GetAcceleratorsMode() && WINDOW_KEY_PRESSED == keyboard_action)
	{
		for (u32 i = 0; i < m_TabsArr.size(); ++i)
		{
			if (m_TabsArr[i]->IsAccelerator(dik) )
			{
				SetActiveTab(m_TabsArr[i]->m_btn_id);
				return	true;
			}
		}
	}
	return false;
}

bool operator == (const CUITabButton* btn, const shared_str& id)
{
	return (btn->m_btn_id==id);
}

CUITabButton* CUITabControl::GetButtonById(const shared_str& id)
{ 
	TABS_VECTOR::const_iterator it = std::find(m_TabsArr.begin(), m_TabsArr.end(), id);
	if(it!=m_TabsArr.end())
		return *it;
	else
		return nullptr;
}

void CUITabControl::ResetTab()
{
	for (u32 i = 0; i < m_TabsArr.size(); ++i)
	{
		m_TabsArr[i]->SetButtonState(CUIButton::BUTTON_NORMAL);
	}
	m_sPushedId		= "";
	m_sPrevPushedId	= "";
}

LPCSTR CUITabControl::GetActiveId_script()
{ 
	LPCSTR res = GetActiveId().c_str();
	return res;
}

void CUITabControl::Enable(bool status)
{
	for(u32 i=0; i<m_TabsArr.size(); ++i)
		m_TabsArr[i]->Enable(status);

	inherited::Enable(status);
}

#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>
using namespace luabind;

#pragma optimize("s",on)
void CUITabControl::script_register(lua_State *L)
{
	module(L)
		[
		class_<CUITabControl, CUIWindow>("CUITabControl")
			.def(constructor<>())
			.def("AddItem", (bool (CUITabControl::*)(CUITabButton*))(&CUITabControl::AddItem), adopt<2>())
			.def("AddItem", (bool (CUITabControl::*)(LPCSTR, LPCSTR, Fvector2, Fvector2))	&CUITabControl::AddItem)
			.def("RemoveAll", &CUITabControl::RemoveAll)
			.def("GetActiveId", &CUITabControl::GetActiveId_script)
			.def("GetTabsCount", &CUITabControl::GetTabsCount)
			.def("SetActiveTab", &CUITabControl::SetActiveTab_script)
			.def("GetButtonById", &CUITabControl::GetButtonById_script),

		class_<CUITabButton, CUIButton>("CUITabButton")
			.def(constructor<>())
		];

}