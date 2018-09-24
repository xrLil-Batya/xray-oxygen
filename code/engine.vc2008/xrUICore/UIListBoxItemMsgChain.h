#pragma once
#include "UIListBoxItem.h"

class UI_API CUIListBoxItemMsgChain : public CUIListBoxItem
{
	using inherited = CUIListBoxItem;
public:
					CUIListBoxItemMsgChain	(float height) : CUIListBoxItem(height) {};
	virtual			~CUIListBoxItemMsgChain	() {};
	virtual bool	OnMouseDown				(int mouse_btn);
};

