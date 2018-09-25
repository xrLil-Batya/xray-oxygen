#pragma once
#include "UIWindow.h"
#include "UIStatic.h"
#include "UIProgressBar.h"

class CUICarPanel : public CUIWindow
{
private:
	typedef CUIWindow inherited;

	CUIStatic			UIStaticCarHealth;
	CUIProgressBar		UICarHealthBar;
public:

	// Установить 
	void				SetCarHealth(float value);
	void				SetSpeed(float speed);
	void				SetRPM(float rmp);
	void				Init(float x, float y, float width, float height);
};