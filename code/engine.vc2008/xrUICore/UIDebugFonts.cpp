#include "StdAfx.h"
#include "UIDebugFonts.h"
#include "UIDialogHolder.h"


CUIDebugFonts::CUIDebugFonts()
{
	AttachChild(&m_background);
	InitDebugFonts(Frect().set(0.0f, 0.0f, UI_BASE_WIDTH, UI_BASE_HEIGHT));
}

CUIDebugFonts::~CUIDebugFonts()
{
}

void CUIDebugFonts::InitDebugFonts(Frect r)
{
	CUIDialogWnd::SetWndRect(r);

	FillUpList();

	m_background.SetWndRect	(r);
	m_background.InitTexture("ui\\ui_debug_font");
}

bool CUIDebugFonts::OnKeyboardAction(u8 dik, EUIMessages keyboard_action)
{
	switch (dik)
	{
	case VK_ESCAPE: HideDialog();	break;
	case VK_F12: return false;		break;
	}

    return true;
}
#include "../xrEngine/string_table.h"

void CUIDebugFonts::FillUpList()
{
	Fvector2 pos, sz;
	pos.set(0.0f, 0.0f);
	sz.set(UI_BASE_WIDTH, UI_BASE_HEIGHT);
	string256 str;
	for (auto ppFont : UI().Font().FontVect)
	{
		if (!ppFont.first)
			continue;

		xr_sprintf(str, "%s:%s", (ppFont.first)->m_font_name.c_str(), CStringTable().translate("Test_Font_String").c_str());

		CUITextWnd* pItem = xr_new<CUITextWnd>();
		pItem->SetWndPos			(pos);
		pItem->SetWndSize			(sz);
		pItem->SetFont				(ppFont.first);
		pItem->SetText				(str);
		pItem->SetTextComplexMode	(false);
		pItem->SetVTextAlignment	(valCenter);
		pItem->SetTextAlignment		(CGameFont::alCenter);
		pItem->AdjustHeightToText	();
		pItem->SetAutoDelete		(true);

		pos.y += pItem->GetHeight() + 20.0f;

		AttachChild(pItem);
	}
}
