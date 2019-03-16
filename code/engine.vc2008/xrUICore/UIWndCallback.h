#pragma once
#include "linker.h"
struct SCallbackInfo;
class CUIWindow;

class UI_API CUIWndCallback
{
public:
	using void_function = xrDelegate<void(CUIWindow*,void*)>;
private:
	using CALLBACKS = xr_vector<SCallbackInfo*>;
private:
			CALLBACKS			m_callbacks;
			SCallbackInfo*		NewCallback			();


public:
	virtual						~CUIWndCallback		();
	virtual void				OnEvent				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			void				Register			(CUIWindow* pChild);
			void				AddCallback			(CUIWindow* pWnd, s16 evt, const void_function &f);
			void				AddCallbackStr		(const shared_str& control_id, s16 evt, const void_function &f);
};