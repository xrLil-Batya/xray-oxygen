#include "stdafx.h"
#include "ScriptXmlInit.h"
#include "../xrUICore/UIXmlInit.h"
#include "../xrUICore/UITextureMaster.h"
#include "../xrUICore/UICheckButton.h"
#include "../xrUICore/UISpinNum.h"
#include "../xrUICore/UISpinText.h"
#include "../xrUICore/UIComboBox.h"
#include "../xrUICore/UITabControl.h"
#include "../xrUICore/UIFrameWindow.h"
#include "../xrUICore/UIKeyBinding.h"
#include "../xrUICore/UIEditBox.h"
#include "../xrUICore/UIAnimatedStatic.h"
#include "../xrUICore/UITrackBar.h"
#include "../xrUICore/UIMMShniaga.h"
#include "../xrUICore/UIScrollView.h"
#include "../xrUICore/UIProgressBar.h"

#include <luabind/luabind.hpp>
using namespace luabind;

void _attach_child(CUIWindow* _child, CUIWindow* _parent)
{
	if (!_parent)
		return;

	_child->SetAutoDelete(true);
	CUIScrollView* _parent_scroll = smart_cast<CUIScrollView*>(_parent);
	if (_parent_scroll)
		_parent_scroll->AddWindow(_child, true);
	else
		_parent->AttachChild(_child);
}

void CScriptXmlInit::ParseFile(LPCSTR xml_file)
{
	m_xml.Load(CONFIG_PATH, UI_PATH, xml_file);
}

void CScriptXmlInit::InitWindow(LPCSTR path, int index, CUIWindow* pWnd)
{
	CUIXmlInit::InitWindow(m_xml, path, index, pWnd);
}

CUIFrameWindow*	CScriptXmlInit::InitFrame(LPCSTR path, CUIWindow* parent)
{
	CUIFrameWindow* pWnd = xr_new<CUIFrameWindow>();
	CUIXmlInit::InitFrameWindow(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIFrameLineWnd* CScriptXmlInit::InitFrameLine(LPCSTR path, CUIWindow* parent)
{
	CUIFrameLineWnd* pWnd = xr_new<CUIFrameLineWnd>();
	CUIXmlInit::InitFrameLine(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIEditBox* CScriptXmlInit::InitEditBox(LPCSTR path, CUIWindow* parent)
{
	CUIEditBox* pWnd = xr_new<CUIEditBox>();
	CUIXmlInit::InitEditBox(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIStatic* CScriptXmlInit::InitStatic(LPCSTR path, CUIWindow* parent)
{
	CUIStatic* pWnd = xr_new<CUIStatic>();
	CUIXmlInit::InitStatic(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUITextWnd* CScriptXmlInit::InitTextWnd(LPCSTR path, CUIWindow* parent)
{
	CUITextWnd* pWnd = xr_new<CUITextWnd>();
	CUIXmlInit::InitTextWnd(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIStatic* CScriptXmlInit::InitAnimStatic(LPCSTR path, CUIWindow* parent)
{
	CUIAnimatedStatic* pWnd = xr_new<CUIAnimatedStatic>();
	CUIXmlInit::InitAnimatedStatic(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIStatic* CScriptXmlInit::InitSleepStatic(LPCSTR path, CUIWindow* parent)
{
	CUISleepStatic* pWnd = xr_new<CUISleepStatic>();
	CUIXmlInit::InitSleepStatic(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIScrollView* CScriptXmlInit::InitScrollView(LPCSTR path, CUIWindow* parent)
{
	CUIScrollView* pWnd = xr_new<CUIScrollView>();
	CUIXmlInit::InitScrollView(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIListBox*	CScriptXmlInit::InitListBox(LPCSTR path, CUIWindow* parent)
{
	CUIListBox* pWnd = xr_new<CUIListBox>();
	CUIXmlInit::InitListBox(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUICheckButton* CScriptXmlInit::InitCheck(LPCSTR path, CUIWindow* parent)
{
	CUICheckButton* pWnd = xr_new<CUICheckButton>();
	CUIXmlInit::InitCheck(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUISpinNum* CScriptXmlInit::InitSpinNum(LPCSTR path, CUIWindow* parent)
{
	CUISpinNum* pWnd = xr_new<CUISpinNum>();
	CUIXmlInit::InitSpin(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUISpinFlt* CScriptXmlInit::InitSpinFlt(LPCSTR path, CUIWindow* parent)
{
	CUISpinFlt* pWnd = xr_new<CUISpinFlt>();
	CUIXmlInit::InitSpin(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUISpinText* CScriptXmlInit::InitSpinText(LPCSTR path, CUIWindow* parent)
{
	CUISpinText* pWnd = xr_new<CUISpinText>();
	CUIXmlInit::InitSpin(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIComboBox* CScriptXmlInit::InitComboBox(LPCSTR path, CUIWindow* parent)
{
	CUIComboBox* pWnd = xr_new<CUIComboBox>();
	CUIXmlInit::InitComboBox(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUI3tButton* CScriptXmlInit::Init3tButton(LPCSTR path, CUIWindow* parent)
{
	CUI3tButton* pWnd = xr_new<CUI3tButton>();
	CUIXmlInit::Init3tButton(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUITabControl* CScriptXmlInit::InitTab(LPCSTR path, CUIWindow* parent)
{
	CUITabControl* pWnd = xr_new<CUITabControl>();
	CUIXmlInit::InitTabControl(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);
	return pWnd;
}

CUIMMShniaga* CScriptXmlInit::InitMMShniaga(LPCSTR path, CUIWindow* parent)
{
	CUIMMShniaga* pWnd = xr_new<CUIMMShniaga>();
	pWnd->InitShniaga(m_xml, path);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIWindow* CScriptXmlInit::InitKeyBinding(LPCSTR path, CUIWindow* parent) 
{
	CUIKeyBinding* pWnd = xr_new<CUIKeyBinding>();
	pWnd->InitFromXml(m_xml, path);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUITrackBar* CScriptXmlInit::InitTrackBar(LPCSTR path, CUIWindow* parent) 
{
	CUITrackBar* pWnd = xr_new<CUITrackBar>();
	CUIXmlInit::InitTrackBar(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

CUIProgressBar* CScriptXmlInit::InitProgressBar(LPCSTR path, CUIWindow* parent)
{
	CUIProgressBar* pWnd = xr_new<CUIProgressBar>();
	CUIXmlInit::InitProgressBar(m_xml, path, 0, pWnd);
	_attach_child(pWnd, parent);

	return pWnd;
}

bool CScriptXmlInit::NodeExists(LPCSTR path, int index)
{
	// Author: SeargeDP
	// Refactoring: FX
	return m_xml.NavigateToNode(path, index);
}


#pragma optimize("gyts",on)
void CScriptXmlInit::script_register(lua_State *L)
{
	module(L)
		[
			class_<CScriptXmlInit>("CScriptXmlInit")
				.def(constructor<>())
				.def("ParseFile",		&CScriptXmlInit::ParseFile)
				.def("InitWindow",		&CScriptXmlInit::InitWindow)
				.def("InitFrame",		&CScriptXmlInit::InitFrame)
				.def("InitFrameLine",	&CScriptXmlInit::InitFrameLine)
				.def("InitEditBox",		&CScriptXmlInit::InitEditBox)
				.def("InitStatic",		&CScriptXmlInit::InitStatic)
				.def("InitTextWnd",		&CScriptXmlInit::InitTextWnd)
				.def("InitAnimStatic",	&CScriptXmlInit::InitAnimStatic)
				.def("InitSleepStatic",	&CScriptXmlInit::InitSleepStatic)
				.def("Init3tButton",	&CScriptXmlInit::Init3tButton)
				.def("InitCheck",		&CScriptXmlInit::InitCheck)
				.def("InitSpinNum",		&CScriptXmlInit::InitSpinNum)
				.def("InitSpinFlt",		&CScriptXmlInit::InitSpinFlt)
				.def("InitSpinText",	&CScriptXmlInit::InitSpinText)
				.def("InitComboBox",	&CScriptXmlInit::InitComboBox)
				.def("InitTab",			&CScriptXmlInit::InitTab)
				.def("InitTrackBar",	&CScriptXmlInit::InitTrackBar)
				.def("InitKeyBinding",	&CScriptXmlInit::InitKeyBinding)
				.def("InitMMShniaga",	&CScriptXmlInit::InitMMShniaga)
				.def("InitScrollView",	&CScriptXmlInit::InitScrollView)
				.def("InitListBox",		&CScriptXmlInit::InitListBox)
				.def("InitProgressBar",	&CScriptXmlInit::InitProgressBar)
				.def("NodeExists",		&CScriptXmlInit::NodeExists)
		];
}

#include "script_ui_registrator.h"
#include "../xrUICore/MainMenu.h"

#include "UIGame.h"
#include "UI/UIScriptWnd.h"
#include "../xrUICore/UIButton.h"
#include "../xrUICore/UIProgressBar.h"
#include "../xrUICore/UIEditBox.h"
#include "../xrUICore/UIMessageBox.h"
#include "../xrUICore/UIPropertiesBox.h"
#include "../xrUICore/UITabControl.h"
#include "../xrUICore/UIComboBox.h"
#include "../xrUICore/UIOptionsManagerScript.h"

#pragma optimize("gyts",on)
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