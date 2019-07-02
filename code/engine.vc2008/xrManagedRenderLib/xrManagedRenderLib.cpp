#include "stdafx.h"
#include "xrManagedRenderLib.h"
#include "ISpectreRenderLib.h"
#include "../xrEngine/Environment.h"
#include "../xrEngine/LightAnimLibrary.h"
#include "../Include/xrRender/UIRender.h"

Runtime::InteropServices::HandleRef XRay::XRayRenderHost::BuildWindowCore(Runtime::InteropServices::HandleRef hwndParent)
{
	HWND XRayWindow = Device.CreateXRayWindow((HWND)hwndParent.Handle.ToPointer(), (int)Width, (int)Height);
	Device.m_hWnd = XRayWindow;

	RECT rc;
	BOOL bRectResult = GetWindowRect(XRayWindow, &rc);

	u32 dWidth = rc.right - rc.left;
	u32 dHeight = rc.bottom - rc.top;
	psCurrentVidMode[0] = dWidth;
	psCurrentVidMode[1] = dHeight;
	psDeviceFlags.set(rsFullscreen, 0);
	ps_vid_windowtype = 1;

	float fWidth_2 = (float)dWidth / 2;
	float fHeight_2 = (float)dHeight / 2;
	Device.dwWidth = dWidth;
	Device.dwHeight = dHeight;
	Device.fWidth_2 = fWidth_2;
	Device.fHeight_2 = fHeight_2;
	Device.Create(true);
	Device.BeginToWork();

	LALib.OnCreate();
	::Environment().Load();
	UIRender->CreateUIGeom();


	// Must be the last - tick thread is running at this point
	Device.b_is_Active = TRUE;
	return HandleRef(this, IntPtr(XRayWindow));
}

Void XRay::XRayRenderHost::DestroyWindowCore(Runtime::InteropServices::HandleRef hwnd)
{
	UIRender->DestroyUIGeom();
	::Environment().Unload();
	LALib.OnDestroy();
}

System::IntPtr XRay::XRayRenderHost::WndProc(IntPtr hwnd, Int32 msg, IntPtr wParam, IntPtr lParam, Boolean% handled)
{
	LRESULT result = 0;
	if (Device.on_message((HWND)hwnd.ToPointer(), msg, wParam.ToInt64(), lParam.ToInt64(), result))
	{
		handled = true;
		return IntPtr(result);
	}

	return HwndHost::WndProc(hwnd, msg, wParam, lParam, handled);
}

System::Void XRay::XRayRenderHost::OnMouseRightButtonDown(::System::Windows::Input::MouseButtonEventArgs^ e)
{
	HwndHost::OnMouseRightButtonDown(e);
	PlatformUtils.ShowCursor(FALSE);
}

System::Void XRay::XRayRenderHost::OnMouseRightButtonUp(::System::Windows::Input::MouseButtonEventArgs^ e)
{
	HwndHost::OnMouseRightButtonUp(e);
	PlatformUtils.ShowCursor(TRUE);
}

XRay::XRayRenderHost::~XRayRenderHost()
{

}

bool XRay::XRayRenderHost::TabIntoCore(TraversalRequest^ request)
{
	return true;
}

bool XRay::XRayRenderHost::TranslateAcceleratorCore(::System::Windows::Interop::MSG% msg, ModifierKeys modifiers)
{
	::MSG m = ConvertMessage(msg);

	LRESULT result = 0;
	if (Device.on_message(m.hwnd, m.message, m.wParam, m.lParam, result))
	{
		return true;
	}

	return false;
}

bool XRay::XRayRenderHost::OnMnemonicCore(::System::Windows::Interop::MSG% msg, ModifierKeys modifiers)
{
	return false;
}

void XRay::XRayRenderHost::SetOcclusion(XRay::Editor::EditorOcclusion^ InOcclusion)
{
	_editorOcclusion = InOcclusion;
}

void XRay::XRayRenderHost::UpdateEngine()
{
	if (Device.b_is_Active && !EditorMode)
	{
		Device.on_idle();
	}
}

::MSG XRay::XRayRenderHost::ConvertMessage(::System::Windows::Interop::MSG% msg)
{
	::MSG m;
	m.hwnd = (HWND)msg.hwnd.ToPointer();
	m.lParam = (LPARAM)msg.lParam.ToPointer();
	m.message = msg.message;
	m.wParam = (WPARAM)msg.wParam.ToPointer();

	m.time = msg.time;

	POINT pt;
	pt.x = msg.pt_x;
	pt.y = msg.pt_y;
	m.pt = pt;

	return m;
}


void XRay::IManagedRenderLib::RenderTick()
{
	Device.on_idle();
}
