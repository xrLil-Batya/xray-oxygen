#pragma once
#include "../xrUICore/UIWindow.h"
#include "../xrUICore/xrUIXmlParser.h"

class CUIStatic;
class CUITextWnd;
struct GAME_NEWS_DATA;

class CUINewsItemWnd :public CUIWindow
{
	using inherited = CUIWindow;

	CUITextWnd*				m_UIDate;
	CUITextWnd*				m_UICaption;
	CUITextWnd*				m_UIText;
	CUIStatic*				m_UIImage;

public:
					CUINewsItemWnd		();
	virtual			~CUINewsItemWnd		();
			void	Init				(CUIXml& uiXml, LPCSTR start_from);
			void	Setup				(GAME_NEWS_DATA& news_data);
	virtual	void	Update				(){};
};