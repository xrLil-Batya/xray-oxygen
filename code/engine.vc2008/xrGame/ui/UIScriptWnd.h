#pragma once
#include "../xrUICore/UIDialogWnd.h"
#include "../../xrServerEntities/script_space_forward.h"
#include "../../xrScripts/export/script_export_space.h"

struct SCallbackInfo;

class CUIDialogWndEx :public CUIDialogWnd, public DLL_Pure
{
	using inherited = CUIDialogWnd;
	using CALLBACKS = xr_vector<SCallbackInfo*>;

private:
			CALLBACKS			m_callbacks;
	virtual void				SendMessageToWnd			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			SCallbackInfo*		NewCallback			();
protected:
			bool				Load				(LPCSTR xml_name);

public:
			void				Register			(CUIWindow* pChild);
			void				Register			(CUIWindow* pChild, LPCSTR name);
								CUIDialogWndEx		();
	virtual						~CUIDialogWndEx		();
			void				AddCallback			(LPCSTR control_id, s16 event, const luabind::functor<void> &lua_function);
			void				AddCallback			(LPCSTR control_id, s16 event, const luabind::functor<void> &functor, const luabind::object &object);
	virtual void				Update				();
	virtual bool				OnKeyboardAction			(u8 dik, EUIMessages keyboard_action);
	virtual bool				Dispatch			(int cmd, int param)				{return true;}
	
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
