#include "stdafx.h"
#include "UIWindow.h"
#include "UICursor.h"
#include "MainMenu.h"
#include "ui_base.h"
#include "../Include/xrRender/DebugRender.h"
#include "../Include/xrRender/UIRender.h"

xr_vector<Frect> g_wnds_rects;
UI_API BOOL g_show_wnd_rect2 = FALSE;

void UI_API clean_wnd_rects()
{
#ifdef DEBUG
	DRender->DestroyDebugShader(IDebugRender::dbgShaderWindow);
#endif // DEBUG
}

void add_rect_to_draw(Frect r)
{
	g_wnds_rects.push_back(r);
}

void draw_rect(Frect& r, u32 color)
{
#ifdef DEBUG

	DRender->SetDebugShader(IDebugRender::dbgShaderWindow);

	UIRender->StartPrimitive	(5, IUIRender::ptLineStrip, UI().m_currentPointType);

	UIRender->PushPoint(r.lt.x, r.lt.y, 0, color, 0,0);
	UIRender->PushPoint(r.rb.x, r.lt.y, 0, color, 0,0);
	UIRender->PushPoint(r.rb.x, r.rb.y, 0, color, 0,0);
	UIRender->PushPoint(r.lt.x, r.rb.y, 0, color, 0,0);
	UIRender->PushPoint(r.lt.x, r.lt.y, 0, color, 0,0);

	UIRender->FlushPrimitive();
	
#endif // DEBUG
}

void UI_API draw_wnds_rects()
{
	if(g_wnds_rects.empty())	return;

	xr_vector<Frect>::iterator it = g_wnds_rects.begin();
	xr_vector<Frect>::iterator it_e = g_wnds_rects.end();

	for(;it!=it_e;++it)
	{
		Frect& r = *it;
		UI().ClientToScreenScaled(r.lt, r.lt.x, r.lt.y);
		UI().ClientToScreenScaled(r.rb, r.rb.x, r.rb.y);
		draw_rect				(r,color_rgba(255,0,0,255));
	};

	g_wnds_rects.clear();
}

void CUIWindow::SetPPMode()
{
	m_bPP					= true;
	MainMenu()->RegisterPPDraw	(this);
	Show					(false);
};

void CUIWindow::ResetPPMode()
{
	if(	GetPPMode() ){
		MainMenu()->UnregisterPPDraw	(this);
		m_bPP							= false;
	}
}

CUIWindow::CUIWindow()
:m_pParentWnd(nullptr),
m_pMouseCapturer(nullptr),
m_pMessageTarget(nullptr),
m_pKeyboardCapturer(nullptr),
m_bAutoDelete(false),
m_bCursorOverWindow(false),
m_bPP(false),
m_dwFocusReceiveTime(0),
m_bCustomDraw(false)
{
	Show					(true);
	Enable					(true);
#ifdef LOG_ALL_WNDS
	ListWndCount++;
	m_dbg_id = ListWndCount;
	dbg_list_wnds.push_back(DBGList());
	dbg_list_wnds.back().num		= m_dbg_id;
	dbg_list_wnds.back().closed		= false;
#endif
}

CUIWindow::~CUIWindow()
{
	VERIFY( !(GetParent()&&IsAutoDelete()) );

	CUIWindow* parent	= GetParent();
	bool ad				= IsAutoDelete();
	if( parent && !ad )
		parent->CUIWindow::DetachChild( this );

	DetachAll();

	if(	GetPPMode() )
		MainMenu()->UnregisterPPDraw	(this);
}

void CUIWindow::Draw()
{
	for (CUIWindow* pWnd : m_ChildWndList)
	{
		if (!pWnd->IsShown())		continue;
		if (pWnd->GetCustomDraw())	continue;
		pWnd->Draw();
	}
#ifdef DEBUG
	if(g_show_wnd_rect2){
		Frect r;
		GetAbsoluteRect(r);
		add_rect_to_draw(r);
	}
#endif
}

void CUIWindow::Draw(float x, float y)
{
	SetWndPos		(Fvector2().set(x,y));
	Draw			();
}

void CUIWindow::Update()
{
	if (GetUICursor().IsVisible())
	{
		bool cursor_on_window;

		Fvector2			temp = GetUICursor().GetCursorPosition();
		Frect				r;
		GetAbsoluteRect		(r);
		cursor_on_window	= !!r.in(temp);
		// RECEIVE and LOST focus
		if(m_bCursorOverWindow != cursor_on_window)
		{
			if(cursor_on_window)
				OnFocusReceive();			
			else
				OnFocusLost();			
		}
	}
	
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it){
		if(!(*it)->IsShown()) continue;
			(*it)->Update();
	}
}

void CUIWindow::AttachChild(CUIWindow* pChild)
{
	R_ASSERT(pChild);
	if(!pChild) return;
	
	R_ASSERT( !IsChild(pChild) );
	pChild->SetParent(this);
	m_ChildWndList.push_back(pChild);
}

void CUIWindow::DetachChild(CUIWindow* pChild)
{
	R_ASSERT(pChild);
	if(nullptr==pChild)
		return;
	
	if(m_pMouseCapturer == pChild)
		SetCapture(pChild, false);

	WINDOW_LIST_it it		= std::find(m_ChildWndList.begin(),m_ChildWndList.end(),pChild); 
	R_ASSERT				(it!=m_ChildWndList.end());
	m_ChildWndList.erase	(it);

	pChild->SetParent		(nullptr);

	if(pChild->IsAutoDelete())
		xr_delete(pChild);
}

void CUIWindow::DetachAll()
{
	while( !m_ChildWndList.empty() ){
		DetachChild( m_ChildWndList.back() );	
	}
}

void CUIWindow::GetAbsoluteRect(Frect& r) 
{
	if(!GetParent())
	{
		GetWndRect		(r);
		return;
	}
	GetParent()->GetAbsoluteRect(r);

	Frect			rr;
	GetWndRect		(rr);
	r.left			+= rr.left;
	r.top			+= rr.top;
	r.right			= r.left + GetWidth();
	r.bottom		= r.top	+ GetHeight();
}

//реакция на мышь
//координаты курсора всегда, кроме начального вызова 
//задаются относительно текущего окна

#define DOUBLE_CLICK_TIME 250

bool CUIWindow::OnMouseAction(float x, float y, EUIMessages mouse_action)
{	
	Frect	wndRect = GetWndRect();

	cursor_pos.x = x;
	cursor_pos.y = y;


	if( WINDOW_LBUTTON_DOWN == mouse_action )
	{
		static u32 _last_db_click_frame		= 0;
		u32 dwCurTime						= Device.dwTimeContinual;

		if( (_last_db_click_frame!=Device.dwFrame) && (dwCurTime-m_dwLastClickTime < DOUBLE_CLICK_TIME) )
		{
            mouse_action			= WINDOW_LBUTTON_DB_CLICK;
			_last_db_click_frame	= Device.dwFrame;
		}

		m_dwLastClickTime = dwCurTime;
	}

	if(!GetParent())
	{
		if(!wndRect.in(cursor_pos))
            return false;
		//получить координаты относительно окна
		cursor_pos.x -= wndRect.left;
		cursor_pos.y -= wndRect.top;
	}


	//если есть дочернее окно,захватившее мышь, то
	//сообщение направляем ему сразу
	if(m_pMouseCapturer)
	{
		m_pMouseCapturer->OnMouseAction(cursor_pos.x - m_pMouseCapturer->GetWndRect().left, 
								  cursor_pos.y - m_pMouseCapturer->GetWndRect().top, 
								  mouse_action);
		return true;
	}

	// handle any action
	switch (mouse_action){
		case WINDOW_MOUSE_MOVE:
			OnMouseMove();							break;
		case WINDOW_MOUSE_WHEEL_DOWN:
			OnMouseScroll(WINDOW_MOUSE_WHEEL_DOWN); break;
		case WINDOW_MOUSE_WHEEL_UP:
			OnMouseScroll(WINDOW_MOUSE_WHEEL_UP);	break;
		case WINDOW_LBUTTON_DOWN:
			if(OnMouseDown(VK_LBUTTON))				return true;	break;
		case WINDOW_RBUTTON_DOWN:
			if(OnMouseDown(VK_RBUTTON))				return true;	break;
		case WINDOW_CBUTTON_DOWN:
			if(OnMouseDown(VK_MBUTTON))				return true;	break;
		case WINDOW_LBUTTON_DB_CLICK:
			if (OnDbClick())						return true;	break;
		default:
            break;
	}

	//Проверка на попадание мыши в окно,
	//происходит в обратном порядке, чем рисование окон
	//(последние в списке имеют высший приоритет)
	WINDOW_LIST::reverse_iterator it = m_ChildWndList.rbegin();

	for(; it!=m_ChildWndList.rend(); ++it)
	{
		CUIWindow* w	= (*it);
		Frect wndRect	= w->GetWndRect();
		if (wndRect.in(cursor_pos) )
		{
			if(w->IsEnabled())
			{
				if( w->OnMouseAction(cursor_pos.x -w->GetWndRect().left, 
							   cursor_pos.y -w->GetWndRect().top, mouse_action))return true;
			}
		}
		else if (w->IsEnabled() && w->CursorOverWindow())
		{
			if( w->OnMouseAction(cursor_pos.x -w->GetWndRect().left, 
						   cursor_pos.y -w->GetWndRect().top, mouse_action))return true;
		}
	}

	return false;
}


void CUIWindow::OnMouseMove(){
}

void CUIWindow::OnMouseScroll(float iDirection){
}

bool CUIWindow::OnDbClick(){
	if (GetMessageTarget())
		GetMessageTarget()->SendMessageToWnd(this, WINDOW_LBUTTON_DB_CLICK);
	return false;
}

bool CUIWindow::OnMouseDown(int mouse_btn){
	return false;
}

void CUIWindow::OnMouseUp(int mouse_btn){
}

void CUIWindow::OnFocusReceive()
{
	m_dwFocusReceiveTime	= Device.dwTimeGlobal;
	m_bCursorOverWindow		= true;	

	if (GetMessageTarget())
        GetMessageTarget()->SendMessageToWnd(this, WINDOW_FOCUS_RECEIVED, nullptr);
}

void CUIWindow::OnFocusLost()
{
	m_dwFocusReceiveTime	= 0;
	m_bCursorOverWindow		= false;	

	if (GetMessageTarget())
        GetMessageTarget()->SendMessageToWnd(this, WINDOW_FOCUS_LOST, nullptr);
}


//Сообщение, посылаемое дочерним окном,
//о том, что окно хочет захватить мышь,
//все сообщения от нее будут направляться только
//ему в независимости от того где мышь
void CUIWindow::SetCapture(CUIWindow *pChildWindow, bool capture_status)
{
	if(GetParent())
	{
		GetParent()->SetCapture(this, capture_status);
	}

	if(capture_status)
	{
		//оповестить дочернее окно о потере фокуса мыши
		if(nullptr!=m_pMouseCapturer)
			m_pMouseCapturer->SendMessageToWnd(this, WINDOW_MOUSE_CAPTURE_LOST);

		m_pMouseCapturer = pChildWindow;
	}
	else
	{
			m_pMouseCapturer = nullptr;
	}
}


//реакция на клавиатуру
bool CUIWindow::OnKeyboardAction(u8 dik, EUIMessages keyboard_action)
{
	bool result;

	//если есть дочернее окно,захватившее клавиатуру, то
	//сообщение направляем ему сразу
	if(nullptr!=m_pKeyboardCapturer)
	{
		result = m_pKeyboardCapturer->OnKeyboardAction(dik, keyboard_action);
		
		if(result) return true;
	}

	WINDOW_LIST::reverse_iterator it = m_ChildWndList.rbegin();

	for(; it!=m_ChildWndList.rend(); ++it)
	{
		if((*it)->IsEnabled())
		{
			result = (*it)->OnKeyboardAction(dik, keyboard_action);
			
			if(result)	return true;
		}
	}
	return false;
}

bool CUIWindow::OnKeyboardHold(u8 dik)
{
	bool result;

	if(nullptr!=m_pKeyboardCapturer)
	{
		result = m_pKeyboardCapturer->OnKeyboardHold(dik);
		
		if(result) return true;
	}

	WINDOW_LIST::reverse_iterator it = m_ChildWndList.rbegin();

	for(; it!=m_ChildWndList.rend(); ++it)
	{
		if((*it)->IsEnabled())
		{
			result = (*it)->OnKeyboardHold(dik);
			
			if(result)	return true;
		}
	}

	return false;
}

void CUIWindow::SetKeyboardCapture(CUIWindow* pChildWindow, bool capture_status)
{
	if(GetParent())
		GetParent()->SetKeyboardCapture(this, capture_status);

	if(capture_status)
	{
		//оповестить дочернее окно о потере фокуса клавиатуры
		if(m_pKeyboardCapturer)
			m_pKeyboardCapturer->SendMessageToWnd(this, WINDOW_KEYBOARD_CAPTURE_LOST);
			
		m_pKeyboardCapturer = pChildWindow;
	}
	else
		m_pKeyboardCapturer = nullptr;
}


//обработка сообщений 
void CUIWindow::SendMessageToWnd(CUIWindow *pWnd, s16 msg, void *pData)
{
	//оповестить дочерние окна
	for(CUIWindow* it: m_ChildWndList)
	{
		if(it->IsEnabled())
			it->SendMessageToWnd(pWnd,msg,pData);
	}
}

CUIWindow* CUIWindow::GetCurrentMouseHandler()
{
	return GetTop()->GetChildMouseHandler();
}

CUIWindow* CUIWindow::GetChildMouseHandler(){
	CUIWindow* pWndResult;
	WINDOW_LIST::reverse_iterator it = m_ChildWndList.rbegin();

	for(; it!=m_ChildWndList.rend(); ++it)
	{
		Frect wndRect = (*it)->GetWndRect();
		// very strange code.... i can't understand difference between
		// first and second condition. I Got It from OnMouseAction() method;
		if (wndRect.in(cursor_pos) )
		{
			if((*it)->IsEnabled())
			{
				return pWndResult = (*it)->GetChildMouseHandler();				
			}
		}
		else if ((*it)->IsEnabled() && (*it)->CursorOverWindow())
		{
			return pWndResult = (*it)->GetChildMouseHandler();
		}
	}

    return this;
}

//для перевода окна и потомков в исходное состояние
void CUIWindow::Reset()
{
	m_pMouseCapturer = nullptr;
}

void CUIWindow::ResetAll()
{
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
		(*it)->Reset();
	}
}

CUIWindow* CUIWindow::GetMessageTarget()
{
	return m_pMessageTarget?m_pMessageTarget:GetParent();
}

bool CUIWindow::IsChild(CUIWindow *pPossibleChild) const
{
	WINDOW_LIST::const_iterator it = std::find(m_ChildWndList.begin(), m_ChildWndList.end(), pPossibleChild);
	return it != m_ChildWndList.end();
}


CUIWindow*	CUIWindow::FindChild(const shared_str name)
{
	if(WindowName()==name)
		return this;

	WINDOW_LIST::const_iterator it = m_ChildWndList.begin();
	WINDOW_LIST::const_iterator it_e = m_ChildWndList.end();
	for(;it!=it_e;++it){
		CUIWindow* pRes = (*it)->FindChild(name);
		if(pRes != nullptr)
			return pRes;
	}
	return nullptr;
}

void CUIWindow::SetParent(CUIWindow* pNewParent) 
{
	R_ASSERT( !(m_pParentWnd && m_pParentWnd->IsChild(this)) );

	m_pParentWnd = pNewParent;
}

void CUIWindow::ShowChildren(bool show){
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)		
			(*it)->Show(show);
}

static bool is_in( Frect const& a, Frect const& b ) //b in a
{
	return (a.x1 < b.x1) && (a.x2 > b.x2) && (a.y1 < b.y1) && (a.y2 > b.y2);
}

UI_API bool fit_in_rect(CUIWindow* w, Frect const& vis_rect, float border, float dx16pos ) //this = hint wnd
{
	float const cursor_height	= 43.0f;
	Fvector2 cursor_pos			= GetUICursor().GetCursorPosition();
	if ( UI().is_widescreen() )
	{
		cursor_pos.x -= dx16pos;
	}

	if ( !vis_rect.in(cursor_pos) )
	{
		return false;
	}

	Frect	rect;
	rect.set( -border, -border, w->GetWidth() - 2.0f*border, w->GetHeight() - 2.0f*border );
	rect.add( cursor_pos.x, cursor_pos.y );

	rect.sub( 0.0f, rect.height() - border );
	if ( !is_in( vis_rect, rect ) ) {	rect.sub( rect.width() - border, 0.0f                   );	}
	if ( !is_in( vis_rect, rect ) ) {	rect.add( 0.0f                 , rect.height() - border );	}
	if ( !is_in( vis_rect, rect ) ) {	rect.add( rect.width() - border, cursor_height          );	}

	float yn = rect.top - vis_rect.height() + rect.height( ) - border + cursor_height;
	if ( !is_in( vis_rect, rect ) ) {	rect.sub( 0.0f                 , yn                     );	}
	if ( !is_in( vis_rect, rect ) ) {	rect.sub( rect.width() - border, 0.0f                   );	}

	w->SetWndPos( rect.lt );
	return true;
}

#include "UIDialogHolder.h"
#include "UIMMShniaga.h"
#include "UITextureMaster.h"
#include "UIScrollView.h"

CFontManager& mngr() 
{
	return UI().Font();
}

// hud font
CGameFont* GetFontSmall()
{
	return mngr().GetFont("stat_font");
}

CGameFont* GetFontMedium()
{
	return mngr().pFontMedium;
}
CGameFont* GetFontDI()
{
	return mngr().pFontDI;
}

int GetARGB(u16 a, u16 r, u16 g, u16 b)
{
	return color_argb(a, r, g, b);
}

const Fvector2* get_wnd_pos(CUIWindow* w)
{
	return &w->GetWndPos();
}
#define GenFontFuncToLua(name) CGameFont* name() { return UI().Font().GetFont(#name); }

GenFontFuncToLua(ui_font_arial_14			)
GenFontFuncToLua(ui_font_arial_21			)
GenFontFuncToLua(ui_font_graffiti19_russian	)
GenFontFuncToLua(ui_font_graffiti22_russian	)
GenFontFuncToLua(ui_font_graffiti32_russian	)
GenFontFuncToLua(ui_font_graffiti50_russian	)
GenFontFuncToLua(ui_font_letterica16_russian)
GenFontFuncToLua(ui_font_letterica18_russian)
GenFontFuncToLua(ui_font_letter_25)

#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIDialogWnd.h"

#include "../../SDK/include/luabind/luabind.hpp"
#include "../../SDK/include/luabind/adopt_policy.hpp"
using namespace luabind;
#pragma optimize("s",on)
void CUIWindow::script_register(lua_State *L)
{
	module(L)
		[
			def("GetARGB",			&GetARGB),
			def("GetFontSmall",		&GetFontSmall),
			def("GetFontMedium",	&GetFontMedium),
			def("GetFontDI",		&GetFontDI),
			def("GetFontArial14",				&ui_font_arial_14			),
			def("GetFontArial21",				&ui_font_arial_21			),
			def("GetFontGraffiti19Russian",		&ui_font_graffiti19_russian	),
			def("GetFontGraffiti22Russian",		&ui_font_graffiti22_russian	),
			def("GetFontGraffiti32Russian",		&ui_font_graffiti32_russian	),
			def("GetFontGraffiti50Russian",		&ui_font_graffiti50_russian	),
			def("GetFontLetterica16Russian",	&ui_font_letterica16_russian),
			def("GetFontLetterica18Russian",	&ui_font_letterica18_russian),
			def("GetFontLetterica25",			&ui_font_letter_25),

			class_<CUIWindow>("CUIWindow")
				.def(constructor<>())
				.def("AttachChild", &CUIWindow::AttachChild, adopt<2>())
				.def("DetachChild", &CUIWindow::DetachChild)
				.def("SetAutoDelete", &CUIWindow::SetAutoDelete)
				.def("IsAutoDelete", &CUIWindow::IsAutoDelete)

				.def("SetWndRect", (void (CUIWindow::*)(const Frect&))	&CUIWindow::SetWndRect_script)
				.def("SetWndPos", (void (CUIWindow::*)(const Fvector2&)) &CUIWindow::SetWndPos_script)
				.def("SetWndSize", (void (CUIWindow::*)(const Fvector2&)) &CUIWindow::SetWndSize_script)
				.def("GetWndPos", &get_wnd_pos)
				.def("GetWidth", &CUIWindow::GetWidth)
				.def("GetHeight", &CUIWindow::GetHeight)

				.def("Enable", &CUIWindow::Enable)
				.def("IsEnabled", &CUIWindow::IsEnabled)
				.def("Show", &CUIWindow::Show)
				.def("IsShown", &CUIWindow::IsShown)

				.def("WindowName", &CUIWindow::WindowName_script)
				.def("SetWindowName", &CUIWindow::SetWindowName)
				.def("SetPPMode", &CUIWindow::SetPPMode)
				.def("ResetPPMode", &CUIWindow::ResetPPMode),

		class_<CDialogHolder>("CDialogHolder")
			.def("AddDialogToRender", &CDialogHolder::AddDialogToRender)
			.def("RemoveDialogToRender", &CDialogHolder::RemoveDialogToRender),

		class_<CUIDialogWnd, CUIWindow>("CUIDialogWnd")
			.def("ShowDialog", &CUIDialogWnd::ShowDialog)
			.def("HideDialog", &CUIDialogWnd::HideDialog)
			.def("GetHolder", &CUIDialogWnd::GetHolder),

		class_<CUIFrameWindow, CUIWindow>("CUIFrameWindow")
			.def(constructor<>())
			.def("SetWidth", &CUIFrameWindow::SetWidth)
			.def("SetHeight", &CUIFrameWindow::SetHeight)
			.def("SetColor", &CUIFrameWindow::SetTextureColor),

		class_<CUIFrameLineWnd, CUIWindow>("CUIFrameLineWnd")
		.def(constructor<>())
		.def("SetWidth", &CUIFrameLineWnd::SetWidth)
		.def("SetHeight", &CUIFrameLineWnd::SetHeight)
		.def("SetColor", &CUIFrameLineWnd::SetTextureColor),

		class_<CUIMMShniaga, CUIWindow>("CUIMMShniaga")
		.enum_("enum_page_id")
		[
			value("epi_main", CUIMMShniaga::epi_main),
			value("epi_new_game", CUIMMShniaga::epi_new_game)
		]
	.def("SetVisibleMagnifier", &CUIMMShniaga::SetVisibleMagnifier)
		.def("SetPage", &CUIMMShniaga::SetPage)
		.def("ShowPage", &CUIMMShniaga::ShowPage),

		class_<CUIScrollView, CUIWindow>("CUIScrollView")
		.def(constructor<>())
		.def("AddWindow", &CUIScrollView::AddWindow)
		.def("RemoveWindow", &CUIScrollView::RemoveWindow)
		.def("Clear", &CUIScrollView::Clear)
		.def("ScrollToBegin", &CUIScrollView::ScrollToBegin)
		.def("ScrollToEnd", &CUIScrollView::ScrollToEnd)
		.def("GetMinScrollPos", &CUIScrollView::GetMinScrollPos)
		.def("GetMaxScrollPos", &CUIScrollView::GetMaxScrollPos)
		.def("GetCurrentScrollPos", &CUIScrollView::GetCurrentScrollPos)
		.def("SetScrollPos", &CUIScrollView::SetScrollPos),

		class_<enum_exporter<EUIMessages> >("ui_events")
		.enum_("events")
		[
			// CUIWindow
			value("WINDOW_LBUTTON_DOWN", int(WINDOW_LBUTTON_DOWN)),
			value("WINDOW_RBUTTON_DOWN", int(WINDOW_RBUTTON_DOWN)),
			value("WINDOW_LBUTTON_UP", int(WINDOW_LBUTTON_UP)),
			value("WINDOW_RBUTTON_UP", int(WINDOW_RBUTTON_UP)),
			value("WINDOW_MOUSE_MOVE", int(WINDOW_MOUSE_MOVE)),
			value("WINDOW_LBUTTON_DB_CLICK", int(WINDOW_LBUTTON_DB_CLICK)),
			value("WINDOW_KEY_PRESSED", int(WINDOW_KEY_PRESSED)),
			value("WINDOW_KEY_RELEASED", int(WINDOW_KEY_RELEASED)),
			value("WINDOW_KEYBOARD_CAPTURE_LOST", int(WINDOW_KEYBOARD_CAPTURE_LOST)),

			// CUIButton
			value("BUTTON_CLICKED", int(BUTTON_CLICKED)),
			value("BUTTON_DOWN", int(BUTTON_DOWN)),

			// CUITabControl
			value("TAB_CHANGED", int(TAB_CHANGED)),

			// CUICheckButton
			value("CHECK_BUTTON_SET", int(CHECK_BUTTON_SET)),
			value("CHECK_BUTTON_RESET", int(CHECK_BUTTON_RESET)),

			// CUIRadioButton
			value("RADIOBUTTON_SET", int(RADIOBUTTON_SET)),

			// CUIScrollBox
			value("SCROLLBOX_MOVE", int(SCROLLBOX_MOVE)),

			// CUIScrollBar
			value("SCROLLBAR_VSCROLL", int(SCROLLBAR_VSCROLL)),
			value("SCROLLBAR_HSCROLL", int(SCROLLBAR_HSCROLL)),

			// CUIListWnd
			value("LIST_ITEM_CLICKED", int(LIST_ITEM_CLICKED)),
			value("LIST_ITEM_SELECT", int(LIST_ITEM_SELECT)),

			// UIPropertiesBox
			value("PROPERTY_CLICKED", int(PROPERTY_CLICKED)),

			// CUIMessageBox
			value("MESSAGE_BOX_OK_CLICKED", int(MESSAGE_BOX_OK_CLICKED)),
			value("MESSAGE_BOX_YES_CLICKED", int(MESSAGE_BOX_YES_CLICKED)),
			value("MESSAGE_BOX_NO_CLICKED", int(MESSAGE_BOX_NO_CLICKED)),
			value("MESSAGE_BOX_CANCEL_CLICKED", int(MESSAGE_BOX_CANCEL_CLICKED)),
			value("MESSAGE_BOX_COPY_CLICKED", int(MESSAGE_BOX_COPY_CLICKED)),
			value("MESSAGE_BOX_QUIT_GAME_CLICKED", int(MESSAGE_BOX_QUIT_GAME_CLICKED)),
			value("MESSAGE_BOX_QUIT_WIN_CLICKED", int(MESSAGE_BOX_QUIT_WIN_CLICKED)),

			value("EDIT_TEXT_COMMIT", int(EDIT_TEXT_COMMIT)),
			// CMainMenu
			value("MAIN_MENU_RELOADED", int(MAIN_MENU_RELOADED))
		]
		];
}
