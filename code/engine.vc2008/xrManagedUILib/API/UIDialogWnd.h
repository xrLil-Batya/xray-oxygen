#pragma once

#include "../xrUICore/UIDialogWnd.h"
#include "../xrUIcore/IGameUI.h"

namespace XRay
{
	public ref class UIDialogWnd abstract
	{
	public:
		UIDialogWnd();
		~UIDialogWnd();

	internal:
		CUIDialogWnd* pNativeLevel;

	public:
	//	CUIGame* GetGameUI() { return (CUIGame*)pUIHud; }

		

	};
}

