#include "stdafx.h"
#ifdef RAW_INPUT_USE
#include "xr_input.h"
#endif
#ifdef XINPUT_USE
#include <XInput.h>
#pragma comment(lib, "xinput.lib")
#endif
extern ENGINE_API BOOL g_bRendering;
static bool bResize = false;

bool CRenderDevice::on_message	(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &result)
{
#ifdef RAW_INPUT_USE
	CInput::DataInput = lParam;
#endif
	switch (uMsg) 
	{
#ifdef XINPUT_USE
	case WM_ACTIVATEAPP: XInputEnable((BOOL)wParam); break; // Controller Input Wrapper
#endif
	case WM_SYSKEYDOWN : return true;
	case WM_ENTERSIZEMOVE: bResize = true; break;
	case WM_TIMER: break;
	case WM_SIZE: break;
	case WM_CLOSE:  if (editor()) break; result = 0; return (true);
	case WM_HOTKEY: break;// prevent 'ding' sounds caused by Alt+key combinations
	case WM_SYSCHAR: result = 0; return true;

	case WM_ACTIVATE : 
	{
		if (editor())
		{
			Device.b_is_Active = TRUE;
			break;
		}
		OnWM_Activate(wParam, lParam);
		return (false);
	}
	case WM_SETCURSOR : 
	{
		if (editor())
			break;

		result			= 1;
		return			(true);
	}
	case WM_EXITSIZEMOVE:
	{
		bResize = false;
		RECT ClientRect;

		GetClientRect(hWnd, &ClientRect);
		LONG width = ClientRect.right - ClientRect.left;
		LONG height = ClientRect.bottom - ClientRect.top;

		if (height >= NULL && width >= NULL) { Device.ResizeProc(height, width); }

		break;
	}
	case WM_SYSCOMMAND : 
	{
		if (editor())
			break;

		bool bRet = false;

		// Prevent moving/sizing and power loss in fullscreen mode
		switch (wParam)
		{
		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_MONITORPOWER:
			result = 1;
			bRet = true;
			break;
		default:
			bRet = false;
		}
		return bRet;
	}
	}

	return (false);
}
//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT	result = 0;
	if (Device.on_message(hWnd, uMsg, wParam, lParam, result)) { return	(result); }

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
