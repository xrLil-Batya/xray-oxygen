#pragma once
#include "script_callback_ex.h"

class CUIWindow;
struct SCallbackInfo
{
	CScriptCallbackEx<void>	m_callback;
	xrDelegate<void(CUIWindow*,void*)> m_cpp_callback;
	
	CUIWindow* 				m_control_ptr;
	shared_str 				m_control_name;
	s16						m_event;

	inline SCallbackInfo():m_control_ptr(NULL),m_event(-1){};
};

struct event_comparer
{
	CUIWindow*			pWnd;
	s16					evt;

	inline event_comparer(CUIWindow* w, s16 e):pWnd(w),evt(e){}
	UI_API bool operator()(SCallbackInfo* i);
};