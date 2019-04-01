////////////////////////////////////////////////////////////////////////////
//	Module 		: UISecondTaskWnd.h
//	Created 	: 30.05.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Secondary Task Wnd class
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../xrUICore/UIWindow.h"
#include "../xrUICore/UIWndCallback.h"

#define PDA_TASK_XML	"pda_tasks.xml"

class CUIXml;
class CUIFrameWindow;
class CUIScrollView;
class CUIStatic;
class CUI3tButton;
class CUICheckButton;
class CUIFrameLineWnd;
class CGameTask;
class UIHint;

class UITaskListWnd : public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow	inherited;

public:
					UITaskListWnd		();
	virtual			~UITaskListWnd		();

			void	init_from_xml		( CUIXml& xml, LPCSTR path );

	virtual bool	OnMouseAction		( float x, float y, EUIMessages mouse_action );
	virtual void 	OnMouseScroll		(float iDirection);
	virtual void	Show				( bool status );
	virtual void 	OnFocusReceive		();
	virtual void	OnFocusLost			();
	virtual void	Update				();
	virtual void	SendMessageToWnd			( CUIWindow* pWnd, s16 msg, void* pData );

			void	UpdateList			();

protected:
	void 	OnBtnClose			( CUIWindow* w, void* d);
	bool 	SortingLessFunction	( CUIWindow* left, CUIWindow* right );

public:
	UIHint*				hint_wnd;

private: // m_
	CUIFrameWindow*		m_background;
	CUIScrollView*		m_list;
	
	CUIStatic*			m_caption;
	CUI3tButton*		m_bt_close;

	float				m_orig_h;

}; // class UITaskListWnd

// -------------------------------------------------------------------------------------------------

class UITaskListWndItem : public CUIWindow
{
private:
	typedef CUIWindow	inherited;

public:
					UITaskListWndItem	();
	virtual			~UITaskListWndItem	();

			bool	init_task			( CGameTask* task, UITaskListWnd* parent );
	IC		u32		get_priority_task	() const;

	virtual void 	OnFocusReceive		();
	virtual void	OnFocusLost			();
	virtual void	Update				();
	virtual void	SendMessageToWnd			( CUIWindow* pWnd, s16 msg, void* pData );
	virtual bool	OnMouseAction				( float x, float y, EUIMessages mouse_action );

private:
			void	hide_hint			();
			void	update_view			();
			void	update_visible_map_spot();

public:
			bool	show_hint_can;
			bool	show_hint;

private: // m_
	CGameTask*		m_task;
	CUI3tButton*	m_name;
	CUIStatic*		m_st_story;
	CUI3tButton*	m_bt_focus;

	enum
	{
		stt_activ = 0,
		stt_unread,
		stt_read,
		stt_count
	};
	u32				m_color_states[stt_count];

}; // class UITaskListWndItem
