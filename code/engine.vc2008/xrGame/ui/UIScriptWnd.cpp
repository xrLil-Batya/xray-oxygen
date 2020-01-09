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

void CUIDialogWndEx::SendMessageToWnd(CUIWindow* pWnd, s16 msg, void* pData)
{
	event_comparer ec(pWnd,msg);

	CALLBACKS::iterator it = std::find_if(m_callbacks.begin(),m_callbacks.end(),ec);
	if(it==m_callbacks.end())
		return inherited::SendMessageToWnd(pWnd, msg, pData);

	((*it)->m_callback)();
}

bool CUIDialogWndEx::Load(LPCSTR xml_name)
{
	return true;
}

SCallbackInfo* CUIDialogWndEx::NewCallback ()
{
	m_callbacks.push_back( xr_new<SCallbackInfo>() );
	return m_callbacks.back();
}

void CUIDialogWndEx::AddCallback (LPCSTR control_id, s16 evt, const luabind::functor<void> &functor, const luabind::object &object)
{
	//for (SCallbackInfo* pCallback : m_callbacks)
	//{
	//	pCallback->m_callback.clear();
	//}

	SCallbackInfo* c	= NewCallback ();
	c->m_callback.set	(functor,object);
	c->m_control_name	= control_id;
	c->m_event			= evt;
}

bool CUIDialogWndEx::OnKeyboardAction(u8 dik, EUIMessages keyboard_action)
{
	return inherited::OnKeyboardAction(dik,keyboard_action);
}

void CUIDialogWndEx::Update()
{
	inherited::Update();
}


//UI-controls
#include "../xrUICore/UIButton.h"
#include "../xrUICore/UIMessageBox.h"
#include "../xrUICore/UIPropertiesBox.h"
#include "../xrUICore/UICheckButton.h"
#include "../xrUICore/UIRadioButton.h"
#include "../xrUICore/UIStatic.h"
#include "../xrUICore/UIEditBox.h"
#include "../xrUICore/UIFrameWindow.h"
#include "../xrUICore/UIFrameLineWnd.h"
#include "../xrUICore/UIProgressBar.h"
#include "../xrUICore/UITabControl.h"

template <typename T>
struct CUIScriptWndWrapperBase : public T, public luabind::wrap_base {
	typedef T inherited;
	typedef CUIScriptWndWrapperBase<T>	self_type;

	virtual bool OnKeyboardAction(u8 dik, EUIMessages keyboard_action)
	{
		return call_member<bool>(this, "OnKeyboard", dik, keyboard_action);
	}
	static bool OnKeyboard_static(inherited* ptr, u8 dik, EUIMessages keyboard_action)
	{
		return ptr->self_type::inherited::OnKeyboardAction(dik, keyboard_action);
	}

	virtual void Update()
	{
		call_member<void>(this, "Update");
	}
	static void Update_static(inherited* ptr)
	{
		ptr->self_type::inherited::Update();
	}

	virtual bool Dispatch(int cmd, int param)
	{
		return call_member<bool>(this, "Dispatch", cmd, param);
	}
	static bool Dispatch_static(inherited* ptr, int cmd, int param)
	{
		return ptr->self_type::inherited::Dispatch(cmd, param);
	}

};

using WrapTypeForScriptWnd = CUIScriptWndWrapperBase<CUIDialogWndEx>;
using BaseTypeForScriptWnd = CUIDialogWndEx;

using export_class_ForScriptWnd = luabind::class_<CUIDialogWndEx, WrapTypeForScriptWnd, luabind::bases<CUIDialogWnd, DLL_Pure> >;


using namespace luabind;

extern export_class_ForScriptWnd script_register_ui_window1(export_class_ForScriptWnd &&);
extern export_class_ForScriptWnd script_register_ui_window2(export_class_ForScriptWnd &&);

#pragma optimize("s",on)
void CUIDialogWndEx::script_register(lua_State *L)
{
	export_class_ForScriptWnd				instance("CUIScriptWnd");

	module(L)
		[
			std::move(script_register_ui_window2(script_register_ui_window1((std::move(instance)))))
			.def("Load", &BaseTypeForScriptWnd::Load)
		];
}

export_class_ForScriptWnd script_register_ui_window1(export_class_ForScriptWnd &&instance)
{
	return std::move(instance)
		.def(constructor<>())
		.def("AddCallback", (void(BaseTypeForScriptWnd::*)(LPCSTR, s16, const luabind::functor<void>&, const luabind::object&))&BaseTypeForScriptWnd::AddCallback)
		.def("Register", (void (BaseTypeForScriptWnd::*)(CUIWindow*, LPCSTR))&BaseTypeForScriptWnd::Register);
}

#pragma optimize("s",on)
export_class_ForScriptWnd script_register_ui_window2(export_class_ForScriptWnd &&instance)
{
	return std::move(instance)
		.def("OnKeyboard", &BaseTypeForScriptWnd::OnKeyboardAction, &WrapTypeForScriptWnd::OnKeyboard_static)
		.def("Update", &BaseTypeForScriptWnd::Update, &WrapTypeForScriptWnd::Update_static)
		.def("Dispatch", &BaseTypeForScriptWnd::Dispatch, &WrapTypeForScriptWnd::Dispatch_static);
}
