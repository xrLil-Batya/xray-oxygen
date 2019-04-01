#include "stdafx.h"
#include "uiCursor.h"
#include "UIStatic.h"
#include "UIBtnHint.h"
#include "../xrGame/Actor_Flags.h"
#include "../xrEngine/xr_input.h"
#include "../xrEngine/IInputReceiver.h"

CUICursor::CUICursor(): m_static(NULL), m_b_use_win_cursor(false)
{    
	bVisible				= false;
	vPrevPos.set			(0.0f, 0.0f);
	vPos.set				(0.f,0.f);
	InitInternal			();
	Device.seqRender.Add	(this,-3/*2*/);
	Device.seqResolutionChanged.Add(this);
}
//--------------------------------------------------------------------
CUICursor::~CUICursor	()
{
	xr_delete				(m_static);
	Device.seqRender.Remove	(this);
	Device.seqResolutionChanged.Remove(this);
}

void CUICursor::OnScreenResolutionChanged()
{
	xr_delete					(m_static);
	InitInternal				();
}

void CUICursor::InitInternal()
{
	m_static					= xr_new<CUIStatic>();

	static const bool bNewCur = strstr(Core.Params, "-hate_gsc_cursor");
	m_static->InitTextureEx		(bNewCur ? "ui\\ui_main_cursor" : "ui\\ui_ani_cursor", "hud\\cursor");

	Frect						rect;
	rect.set					(0.0f,0.0f,40.0f,40.0f);
	m_static->SetTextureRect	(rect);
	Fvector2					sz;
	sz.set						(rect.rb);
	sz.x						*= UI().get_current_kx();

	m_static->SetWndSize		(sz);
	m_static->SetStretchTexture	(true);

	u32 screen_size_x	= GetSystemMetrics( SM_CXSCREEN );
	u32 screen_size_y	= GetSystemMetrics( SM_CYSCREEN );
	m_b_use_win_cursor	= (screen_size_y >=Device.dwHeight && screen_size_x>=Device.dwWidth);
}

//--------------------------------------------------------------------
u32 last_render_frame = 0;
void CUICursor::OnRender	()
{
	g_btnHint->OnRender();
	g_statHint->OnRender();

	if( !IsVisible() ) return;

	if (psActorFlags.test(AF_SHOW_CURPOS))
	{
		CGameFont* F = UI().Font().pFontDI;
		F->SetAligment(CGameFont::alCenter);
		F->SetHeightI(0.02f);
		F->OutSetI(0.f, -0.9f);
		F->SetColor(0xffffffff);
		Fvector2			pt = GetCursorPosition();
		F->OutNext("%f-%f", pt.x, pt.y);
	}

	m_static->SetWndPos	(vPos);
	m_static->Update	();
	m_static->Draw		();
}

Fvector2 CUICursor::GetCursorPosition()
{
	return  vPos;
}

Fvector2 CUICursor::GetCursorPositionDelta()
{
	Fvector2 res_delta;

	res_delta.x = vPos.x - vPrevPos.x;
	res_delta.y = vPos.y - vPrevPos.y;
	return res_delta;
}

void CUICursor::UpdateCursorPosition(int _dx, int _dy)
{
	vPrevPos	= vPos;
	if(m_b_use_win_cursor)
	{
		Ivector2 mousePos;
		pInput->CurrentIR()->IR_GetMousePosReal(mousePos);

		vPos.x = (float)mousePos.x * (UI_BASE_WIDTH / (float)Device.dwWidth);
		vPos.y = (float)mousePos.y * (UI_BASE_HEIGHT / (float)Device.dwHeight);
			
		clamp<float>(vPos.x, 0.0f, UI_BASE_WIDTH);
		clamp<float>(vPos.y, 0.0f, UI_BASE_HEIGHT);
	}
	else
	{
		vPos.x += _dx;
		vPos.y += _dy;
	}
}

void CUICursor::SetUICursorPosition(Fvector2 pos)
{
	vPos		= pos;
	POINT		p;
	p.x			= iFloor(vPos.x / (UI_BASE_WIDTH/(float)Device.dwWidth));
	p.y			= iFloor(vPos.y / (UI_BASE_HEIGHT/(float)Device.dwHeight));

	SetCursorPos(p.x, p.y);
}
