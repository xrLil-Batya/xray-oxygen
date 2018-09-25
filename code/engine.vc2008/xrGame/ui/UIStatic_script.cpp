#include "stdafx.h"
#include "../xrUICore/UIStatic.h"
#include "../xrUICore/UIAnimatedStatic.h"

#include "luabind/luabind.hpp"
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
	];
}

#include "script_ui_registrator.h"
#include "../../xrUICore/MainMenu.h"

#include "UIGame.h"
#include "UIScriptWnd.h"
#include "../../xrUICore/UIButton.h"
#include "../../xrUICore/UIProgressBar.h"
#include "../../xrUICore/UIEditBox.h"
#include "../../xrUICore/UIMessageBox.h"
#include "../../xrUICore/UIPropertiesBox.h"
#include "../../xrUICore/UITabControl.h"
#include "../../xrUICore/UIComboBox.h"
#include "../../xrUICore/UIOptionsManagerScript.h"
#include "ScriptXmlInit.h"

#pragma optimize("s",on)
void UIRegistrator::script_register(lua_State *L)
{
	CUIWindow::script_register(L);
	CUIStatic::script_register(L);
	CUIButton::script_register(L);
	CUIProgressBar::script_register(L);
	CUIComboBox::script_register(L);
	CUIEditBox::script_register(L);
	CUITabControl::script_register(L);
	CUIMessageBox::script_register(L);
	CUIListBox::script_register(L);
	CUIDialogWndEx::script_register(L);
	CUIPropertiesBox::script_register(L);
	CUIOptionsManagerScript::script_register(L);
	CScriptXmlInit::script_register(L);
	CUIGame::script_register(L);

	module(L)
		[
			class_<CGameFont>("CGameFont")
			.enum_("EAligment")
		[
			value("alLeft", u32(CGameFont::alLeft)),
			value("alRight", u32(CGameFont::alRight)),
			value("alCenter", u32(CGameFont::alCenter))
		],

		class_<CMainMenu>("CMainMenu")
		.def("GetEngineBuild", &CMainMenu::GetEngineBuild)
		.def("GetEngineBuildDate", &CMainMenu::GetEngineBuildDate)
		.def("GetGSVer", &CMainMenu::GetGSVer)
		];

	module(L, "main_menu")
		[
			def("get_main_menu", &MainMenu)
		];
}