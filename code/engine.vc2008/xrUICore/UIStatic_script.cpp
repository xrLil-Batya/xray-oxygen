#include "stdafx.h"
#include "UIStatic.h"
//#include "UI3dStatic.h"
#include "UIAnimatedStatic.h"

#include "luabind/luabind.hpp"
#include "../../SDK/include/luabind/adopt_policy.hpp"
using namespace luabind;

#pragma optimize("s",on)

void CUIStatic::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUILines>("CUILines")
		.def("SetFont",				&CUILines::SetFont)
		.def("SetText",				&CUILines::SetText)
		.def("SetTextST",			&CUILines::SetTextST)
		.def("GetText",				&CUILines::GetText)
		.def("SetElipsis",			&CUILines::SetEllipsis)
		.def("SetTextColor",		&CUILines::SetTextColor),


		class_<CUIStatic, CUIWindow>("CUIStatic")
		.def(						constructor<>())
		.def("TextControl",			&CUIStatic::TextItemControl)
		.def("InitTexture",			&CUIStatic::InitTexture )
		.def("SetTextureRect",		&CUIStatic::SetTextureRect_script)
		.def("SetStretchTexture",	&CUIStatic::SetStretchTexture)
		.def("GetTextureRect",		&CUIStatic::GetTextureRect_script),

		class_<CUITextWnd, CUIWindow>("CUITextWnd")
		.def(						constructor<>())
		.def("AdjustHeightToText",	&CUITextWnd::AdjustHeightToText)
		.def("AdjustWidthToText",	&CUITextWnd::AdjustWidthToText)
		.def("SetText",				&CUITextWnd::SetText)
		.def("SetTextST",			&CUITextWnd::SetTextST)
		.def("GetText",				&CUITextWnd::GetText)
		.def("SetFont",				&CUITextWnd::SetFont)
		.def("GetFont",				&CUITextWnd::GetFont)
		.def("SetTextColor",		&CUITextWnd::SetTextColor)
		.def("GetTextColor",		&CUITextWnd::GetTextColor)
		.def("SetTextComplexMode",	&CUITextWnd::SetTextComplexMode)
		.def("SetTextAlignment",	&CUITextWnd::SetTextAlignment)
		.def("SetVTextAlignment",	&CUITextWnd::SetVTextAlignment)
		.def("SetEllipsis",			&CUITextWnd::SetEllipsis)
		.def("SetTextOffset",		&CUITextWnd::SetTextOffset),

		class_<CUISleepStatic, CUIStatic>("CUISleepStatic")
		.def(						constructor<>())
		
		//class_<CUI3dStatic>("CUI3dStatic")
		//	.def(constructor<>())
		//	.def("AttachChild", &CUIWindow::AttachChild, adopt<2>())
		//	.def("DetachChild", &CUIWindow::DetachChild)
		//	.def("SetAutoDelete", &CUIWindow::SetAutoDelete)
		//	.def("IsAutoDelete", &CUIWindow::IsAutoDelete)
		//
		//	.def("SetWndRect", (void (CUIWindow::*)(const Frect&))	&CUIWindow::SetWndRect_script)
		//	.def("SetWndPos", (void (CUIWindow::*)(const Fvector2&)) &CUIWindow::SetWndPos_script)
		//	.def("SetWndSize", (void (CUIWindow::*)(const Fvector2&)) &CUIWindow::SetWndSize_script)
		//	.def("GetWidth", &CUIWindow::GetWidth)
		//	.def("GetHeight", &CUIWindow::GetHeight)
		//
		//	.def("Enable", &CUIWindow::Enable)
		//	.def("IsEnabled", &CUIWindow::IsEnabled)
		//	.def("Show", &CUIWindow::Show)
		//	.def("IsShown", &CUIWindow::IsShown)
		//
		//	.def("WindowName", &CUIWindow::WindowName_script)
		//	.def("SetWindowName", &CUIWindow::SetWindowName)
		//	.def("SetPPMode", &CUIWindow::SetPPMode)
		//	.def("ResetPPMode", &CUIWindow::ResetPPMode)
	];
}
