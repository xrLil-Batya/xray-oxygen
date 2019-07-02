#include "stdafx.h"
#include "../FrayBuildConfig.hpp"
#include "xr_input.h"

extern ENGINE_API BOOL g_bRendering;
static bool bResize = false;

bool CRenderDevice::on_message	(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &result)
{
	switch (uMsg) 
	{
	case WM_SYSKEYDOWN : return true;
	case WM_ENTERSIZEMOVE: bResize = true; break;
	case WM_TIMER: break;
	case WM_CLOSE:  if (editor()) break; result = 0; return (true);
	case WM_HOTKEY: break;// prevent 'ding' sounds caused by Alt+key combinations
	case WM_SYSCHAR: result = 0; return true;
	case WM_INPUT:
	{
		pInput->ProcessInput(lParam);
		return true;
	}

	case WM_ACTIVATE : 
	{
		if (editor())
		{
			Device.b_is_Active = TRUE;
			break;
		}	
		
		PlatformUtils.ShowCursor(wParam == WA_INACTIVE);
		
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
	case WM_SIZE:
	{
		break;
	}
	case WM_EXITSIZEMOVE:
	{
		if (wParam != SIZE_MINIMIZED)
		{
			bResize = false;
			RECT ClientRect;

			GetClientRect(hWnd, &ClientRect);
			LONG width = ClientRect.right - ClientRect.left;
			LONG height = ClientRect.bottom - ClientRect.top;

			if (Device.dwWidth != u32(width) || Device.dwHeight != u32(height))
			{
				if (height >= NULL && width >= NULL)
				{
					Device.ResizeProc(height, width);
					pInput->LockMouse();
				}
			}
		}
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
			break;
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
