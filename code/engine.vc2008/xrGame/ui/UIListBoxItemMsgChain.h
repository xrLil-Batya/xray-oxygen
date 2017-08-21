#pragma once
#include "UIListBoxItem.h"

class CUIListBoxItemMsgChain : public CUIListBoxItem
{
	using inherited = CUIListBoxItem;
public:
					CUIListBoxItemMsgChain	(float height) : CUIListBoxItem(height) {};
	virtual			~CUIListBoxItemMsgChain	() {};
	virtual bool	OnMouseDown				(int mouse_btn);
};

#endif //#ifndef UILISTBOXITEMMSGCHAIN_H_INCLUDED