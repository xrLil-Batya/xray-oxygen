#include "stdafx.h"
#include "UIScriptWnd.h"
#include "object_broker.h"
#include "../callback_info.h"


CUIDialogWndEx::CUIDialogWndEx()
{}

CUIDialogWndEx::~CUIDialogWndEx()
{
	delete_data(m_callbacks);
}

void CUIDialogWndEx::Register			(CUIWindow* pChild)
{
	pChild->SetMessageTarget(this);
}

void CUIDialogWndEx::Register(CUIWindow* pChild, LPCSTR name)
{
	pChild->SetWindowName(name);
	pChild->SetMessageTarget(this);
}

void CUIDialogWndEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	event_comparer ec(pWnd,msg);

	CALLBACKS::iterator it = std::find_if(m_callbacks.begin(),m_callbacks.end(),ec);
	if(it==m_callbacks.end())
		return inherited::SendMessage(pWnd, msg, pData);

	((*it)->m_callback)();
}

bool CUIDialogWndEx::Load(LPCSTR xml_name)
{
	return true;
}

SCallbackInfo*	CUIDialogWndEx::NewCallback ()
{
	m_callbacks.push_back( xr_new<SCallbackInfo>() );
	return m_callbacks.back();
}

void CUIDialogWndEx::AddCallback (LPCSTR control_id, s16 evt, const luabind::functor<void> &functor, const luabind::object &object)
{
	SCallbackInfo* c	= NewCallback ();
	c->m_callback.set	(functor,object);
	c->m_control_name	= control_id;
	c->m_event			= evt;
}

bool CUIDialogWndEx::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{
	return inherited::OnKeyboardAction(dik,keyboard_action);
}

void CUIDialogWndEx::Update()
{
	inherited::Update();
}


//UI-controls
#include "UIButton.h"
#include "UIMessageBox.h"
#include "UIPropertiesBox.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UIStatic.h"
#include "UIEditBox.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIProgressBar.h"
#include "UITabControl.h"
#include "uiscriptwnd_script.h"

using namespace luabind;

extern export_class script_register_ui_window1(export_class &&);
extern export_class script_register_ui_window2(export_class &&);

#pragma optimize("s",on)
void CUIDialogWndEx::script_register(lua_State *L)
{
	export_class				instance("CUIScriptWnd");

	module(L)
		[
			std::move(script_register_ui_window2(script_register_ui_window1((std::move(instance)))))
			.def("Load", &BaseType::Load)
		];
}

export_class script_register_ui_window1(export_class &&instance)
{
	return std::move(instance)
		.def(constructor<>())
		.def("AddCallback", (void(BaseType::*)(LPCSTR, s16, const luabind::functor<void>&, const luabind::object&))&BaseType::AddCallback)
		.def("Register", (void (BaseType::*)(CUIWindow*, LPCSTR))&BaseType::Register);
}

#pragma optimize("s",on)
export_class script_register_ui_window2(export_class &&instance)
{
	return std::move(instance)
		.def("OnKeyboard", &BaseType::OnKeyboardAction, &WrapType::OnKeyboard_static)
		.def("Update", &BaseType::Update, &WrapType::Update_static)
		.def("Dispatch", &BaseType::Dispatch, &WrapType::Dispatch_static);
}
