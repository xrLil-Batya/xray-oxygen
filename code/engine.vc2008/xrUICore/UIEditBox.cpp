// CUIEditBox.cpp: ввод строки с клавиатуры
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIEditBox.h"
#include "UIFrameLineWnd.h"

CUIEditBox::CUIEditBox() : m_frameLine(nullptr)
{
}

void CUIEditBox::InitCustomEdit(Fvector2 pos, Fvector2 size)
{
	if(m_frameLine)
	{
		m_frameLine->SetWndPos			(Fvector2().set(0,0));
		m_frameLine->SetWndSize			(size);
	}
	CUICustomEdit::InitCustomEdit	(pos, size);
}

void CUIEditBox::InitTextureEx(LPCSTR texture, LPCSTR  shader)
{
	if(!m_frameLine)
	{
		m_frameLine = xr_new<CUIFrameLineWnd>();
		AttachChild(m_frameLine);
		m_frameLine->SetAutoDelete(true);
	}
	m_frameLine->InitTexture(texture, shader);
	m_frameLine->SetWndPos			(Fvector2().set(0,0));
	m_frameLine->SetWndSize			(GetWndSize());
}

void CUIEditBox::InitTexture(LPCSTR texture)
{
	InitTextureEx(texture, "hud\\default");
}

void CUIEditBox::SetCurrentOptValue()
{
	CUIOptionsItem::SetCurrentOptValue	();
	SetText								(GetOptStringValue());
}

void CUIEditBox::SaveOptValue()
{
	CUIOptionsItem::SaveOptValue		();
	SaveOptStringValue					(GetText());
}

void CUIEditBox::SaveBackUpOptValue()
{
	CUIOptionsItem::SaveBackUpOptValue	();
	m_opt_backup_value					= GetText();
}

void CUIEditBox::UndoOptValue()
{
	SetText								(m_opt_backup_value.c_str());
	CUIOptionsItem::UndoOptValue		();
}

bool CUIEditBox::IsChangedOptValue() const
{
	return 0 != xr_strcmp(m_opt_backup_value.c_str(), GetText());
}
#include "luabind/luabind.hpp"
using namespace luabind;

#pragma optimize("s",on)
void CUIEditBox::script_register(lua_State *L)
{
	module(L)
		[
		class_<CUICustomEdit, CUIWindow>("CUICustomEdit")
		.def("SetText", &CUICustomEdit::SetText)
		.def("GetText", &CUICustomEdit::GetText)
		.def("CaptureFocus", &CUICustomEdit::CaptureFocus)
		.def("SetNextFocusCapturer", &CUICustomEdit::SetNextFocusCapturer),

		class_<CUIEditBox, CUICustomEdit>("CUIEditBox")
		.def(constructor<>())
		.def("InitTexture", &CUIEditBox::InitTexture)
		];
}