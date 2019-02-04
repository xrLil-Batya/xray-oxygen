#pragma once
#include "../xrUICore/UIDialogWnd.h"
class UIDialogWnd;

namespace XRay
{
	ref class UIDialogWnd
	{
	internal:
		CUIDialogWnd* pNativeObject;
	public:
		delegate void Functor();

	public:

		CUIGame* GameUI() { return g_hud ? HUD().GetGameUI() : nullptr; }

		UIDialogWnd() = default;
		UIDialogWnd(CUIDialogWnd* pObg) : pNativeObject(pObg) {};

		~UIDialogWnd();



	};



}