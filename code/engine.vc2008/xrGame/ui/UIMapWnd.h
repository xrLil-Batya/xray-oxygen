#pragma once
#include "../xrUICore/UIWindow.h"
#include "../xrUICore/UIWndCallback.h"
class CUICustomMap;
class CUIGlobalMap;
class CUIFrameWindow;
class CUIFixedScrollBar;
class CMapActionPlanner;
class CUIStatic;
class CUI3tButton;
class CUIMapLocationHint;
class CMapLocation;
class CMapSpot;
class CGameTask;
class CUIXml;
class UIHint;
class CUIPropertiesBox;
class CUIPdaSpot;
class CUILevelMap;

using GameMaps = xr_map<shared_str, CUICustomMap*>;

class CUIMapWnd: public CUIWindow, public CUIWndCallback
{
	using inherited = CUIWindow;
private:
	bool						m_view_actor;
	Fvector2					m_prev_actor_pos;

private:
	float						m_map_move_step;

	float						m_currentZoom;
	CUIGlobalMap*				m_GlobalMap;
	GameMaps					m_GameMaps;
	
	CUIFrameWindow*				m_UIMainFrame;
	bool						m_scroll_mode;
	CUIFixedScrollBar*				m_UIMainScrollV;
	CUIFixedScrollBar*				m_UIMainScrollH;
	CUIWindow*					m_UILevelFrame;
	CMapActionPlanner*			m_ActionPlanner;
	CUIMapLocationHint*			m_map_location_hint;

	enum EBtnPos
	{
		btn_legend		= 0,
		btn_up			= 1,
		btn_zoom_more	= 2,
		btn_left		= 3,
		btn_actor		= 4,
		btn_right		= 5,
		btn_zoom_less	= 6,
		btn_down		= 7,
		btn_zoom_reset	= 8,
		max_btn_nav		= 9
	};
	CUI3tButton*				m_btn_nav[max_btn_nav];
	CUIStatic*					m_btn_nav_parent;
	u32							m_nav_timing;

	void						UpdateNav				();

	void 				OnBtnLegend_Push		(CUIWindow*, void*);
	void 				OnBtnUp_Push			(CUIWindow*, void*);
	void 				OnBtnZoomMore_Push		(CUIWindow*, void*);

	void 				OnBtnLeft_Push			(CUIWindow*, void*);
	void 				OnBtnActor_Push			(CUIWindow*, void*);
	void 				OnBtnRight_Push			(CUIWindow*, void*);

	void 				OnBtnZoomLess_Push		(CUIWindow*, void*);
	void 				OnBtnDown_Push			(CUIWindow*, void*);
	void 				OnBtnZoomReset_Push		(CUIWindow*, void*);

private:
	void 				OnScrollV				(CUIWindow*, void*);
	void 				OnScrollH				(CUIWindow*, void*);
	
	void						OnToolNextMapClicked	(CUIWindow*, void*);
	void						OnToolPrevMapClicked	(CUIWindow*, void*);

	void						ResetActionPlanner		();

public:
	void						ViewGlobalMap	();
	void						ViewActor		();
	void						ViewZoomIn		();
	void						ViewZoomOut		();

	void						MoveScrollV		( float dy );
	void						MoveScrollH		( float dx );
	// qweasdd: from lost alpha
	bool						ConvertCursorPosToMap(Fvector*, CUICustomMap*);
	void						CreateSpotWindow(Fvector, shared_str);
	CMapLocation*				UnderSpot(Fvector RealPosition, CUICustomMap*);
	//-qweasdd
	void 						ActivatePropertiesBox(CUIWindow* w);
public:
	CUICustomMap*				m_tgtMap;
	Fvector2					m_tgtCenter;
	UIHint*						hint_wnd;

protected:
	void						init_xml_nav			(CUIXml& xml);
	void						ShowHint				(bool extra = false);
	void						Activated				();
	
	CUIPropertiesBox*			m_UIPropertiesBox;
	CUIPdaSpot*					m_UserSpotWnd;
	CMapLocation*				m_cur_location;
	void						ShowSettingsWindow		(u16 id, Fvector position, shared_str levelName);

public:
								CUIMapWnd				();
	virtual						~CUIMapWnd				();

	virtual void				Init					(LPCSTR xml_name, LPCSTR start_from);
	virtual void				Show					(bool status);
	virtual void				Draw					();
	virtual void				Reset					();
	virtual void				Update					();
			void				DrawHint				();

		
			void				MoveMap					(Fvector2 const& pos_delta);
			float				GetZoom					()	{return m_currentZoom;}
			void				SetZoom					(float value);
			bool				UpdateZoom				(bool b_zoom_in);


			void				ShowHintStr				(CUIWindow* parent, LPCSTR text);
			void				ShowHintSpot			(CMapSpot* spot);
			void				ShowHintTask			(CGameTask* task, CUIWindow* owner);

			void				SpotSelected			(CUIWindow* spot);

			void				HideHint				(CUIWindow* parent);
			void				HideCurHint				();
			void				Hint					(const shared_str& text);
	virtual bool				OnMouseAction					(float x, float y, EUIMessages mouse_action);
	virtual bool				OnKeyboardAction				(u8 dik, EUIMessages keyboard_action);
	virtual bool				OnKeyboardHold			(u8 dik);

	virtual void				SendMessageToWnd				(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	void						SetTargetMap			(CUICustomMap* m, bool bZoomIn = false);
	void						SetTargetMap			(CUICustomMap* m, const Fvector2& pos, bool bZoomIn = false);
	void						SetTargetMap			(const shared_str& name, const Fvector2& pos, bool bZoomIn = false);
	void						SetTargetMap			(const shared_str& name, bool bZoomIn = false);

	void						MapLocationRelcase				(CMapLocation* ml);

	Frect						ActiveMapRect			()		{Frect r; m_UILevelFrame->GetAbsoluteRect(r); return r;};
	void						AddMapToRender			(CUICustomMap*);
	void						RemoveMapToRender		(CUICustomMap*);
	CUIGlobalMap*				GlobalMap				()		{return m_GlobalMap;};
	const GameMaps&				GameMaps				()		{return m_GameMaps;};	
	CUICustomMap*				GetMapByIdx				(u16 idx);
	u16							GetIdxByName			(const shared_str& map_name);
	void						UpdateScroll			();
	shared_str					cName					() const	{return "ui_map_wnd";};
};
