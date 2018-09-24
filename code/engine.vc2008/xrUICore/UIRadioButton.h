#pragma once
#include "UITabButton.h"

class UI_API CUIRadioButton : public CUITabButton
{
	using inherited = CUITabButton;
public:
	virtual void InitButton(Fvector2 pos, Fvector2 size);
	virtual void InitTexture(LPCSTR tex_name);
	virtual void SetTextX(float x)	{/*do nothing*/}
};