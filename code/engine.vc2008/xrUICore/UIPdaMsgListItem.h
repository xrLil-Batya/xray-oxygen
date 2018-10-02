#pragma once
#include "UILanimController.h"
#include "UIStatic.h"

class UI_API CUIPdaMsgListItem : public CUIColorAnimConrollerContainer
{
	using inherited = CUIColorAnimConrollerContainer;
public:
			void		InitPdaMsgListItem				(const Fvector2& size);
	virtual void		SetFont							(CGameFont* pFont);
	
	CUIStatic			UIIcon;
	CUITextWnd			UITimeText;
	CUITextWnd			UICaptionText;
	CUITextWnd			UIMsgText;
};