////////////////////////////////////////////////////////////////////////////
//	Module 		: UILogsWnd.h
//	Created 	: 25.04.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Logs (PDA) window class
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../xrUICore/UIWindow.h"
#include "../xrUICore/UIWndCallback.h"

#include "../ai_space.h"
#include "../../xrServerEntities/alife_space.h"
#include "../xrUICore/xrUIXmlParser.h"

class CUITextWnd;
class CUIFrameWindow;
class CUIScrollView;
class CUI3tButton;
class CUICheckButton;
struct GAME_NEWS_DATA;
class CUINewsItemWnd;

class CUILogsWnd : public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow	inherited;

	CUIFrameWindow*		m_background;
	CUIFrameWindow*		m_center_background;

	CUITextWnd*			m_center_caption;
	CUICheckButton*		m_filter_news;
	CUICheckButton*		m_filter_talk;
	
	CUITextWnd*			m_period_caption;
	CUITextWnd*			m_period;

	ALife::_TIME_ID		m_start_game_time;
	ALife::_TIME_ID		m_selected_period;

	CUI3tButton*		m_prev_period;
	CUI3tButton*		m_next_period;
	bool				m_ctrl_press;
	
	CUIScrollView*		m_list;
	u32					m_previous_time;
	bool				m_need_reload;
	WINDOW_LIST			m_items_cache;
	WINDOW_LIST			m_items_ready;
	xr_vector<u32>		m_news_in_queue;

	CUIWindow*			CreateItem			();
	CUIWindow*			ItemFromCache		();
	CUIXml				m_uiXml;

public:
						CUILogsWnd			();
	virtual				~CUILogsWnd			();

			void		Init				();

	virtual void 		Show				( bool status );
	virtual void		Update				();
	virtual void		SendMessageToWnd			( CUIWindow* pWnd, s16 msg, void* pData );

	virtual bool		OnKeyboardAction			(u8 dik, EUIMessages keyboard_action);
	virtual bool		OnKeyboardHold		(u8 dik);

	IC		void		UpdateNews			()	{ m_need_reload = true; }
	void		PerformWork			();

protected:
			void		ReLoadNews			();
			void		AddNewsItem			( GAME_NEWS_DATA& news_data );
	ALife::_TIME_ID		GetShiftPeriod		( ALife::_TIME_ID datetime, int shift_day );

			void 	UpdateChecks	( CUIWindow* w, void* d);
			void 	PrevPeriod		( CUIWindow* w, void* d);
			void 	NextPeriod		( CUIWindow* w, void* d);
	
			void 		on_scroll_keys		( u8 dik );
}; // class CUILogsWnd