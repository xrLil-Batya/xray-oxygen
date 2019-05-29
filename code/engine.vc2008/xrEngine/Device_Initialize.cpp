#include "stdafx.h"
#include "../xrPlay/resource.h"
#include "../include/editor/ide.hpp"

extern LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

#ifdef INGAME_EDITOR
void CRenderDevice::initialize_editor(engine_impl* pEngine)
{
	m_editor_module		= LoadLibrary("editor.dll");
	if (!m_editor_module) {
		Msg				("! cannot load library \"editor.dll\"");
		return;
	}

	m_editor_initialize	= (initialize_function_ptr)GetProcAddress(m_editor_module, "initialize");
	VERIFY				(m_editor_initialize);

	m_editor_finalize	= (finalize_function_ptr)GetProcAddress(m_editor_module, "finalize");
	VERIFY				(m_editor_finalize);

	m_engine			= pEngine;
	m_editor_initialize	(m_editor, (editor::engine*)m_engine);
	VERIFY				(m_editor);

	m_hWnd				= m_editor->view_handle();
	VERIFY				(m_hWnd != INVALID_HANDLE_VALUE);
}
#endif // #ifdef INGAME_EDITOR

void CRenderDevice::Initialize			()
{
	Log							("Initializing Engine...");
	TimerGlobal.Start			();
	TimerMM.Start				();

	// Unless a substitute hWnd has been specified, create a window to render into
    if(m_hWnd == NULL)
		CreateXRayWindow();

	if (m_hWnd == NULL)
	{
		R_ASSERT(m_hWnd);
		return;
	}

    // Save window properties
    m_dwWindowStyle = GetWindowLongPtr( m_hWnd, GWL_STYLE );
}

HWND CRenderDevice::CreateXRayWindow(HWND parent /*= NULL*/, int Width /*= 0*/, int Height /*= 0*/)
{
	LPCSTR wndclass = "_XRAY_1.7";

	int FinalWidth = Width;
	int FinalHeight = Height;

	// Register the windows class
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(0);
	UINT ClassStyle = 0;
	WNDCLASS wndClass = { ClassStyle,
						  WndProc, 0, 0, hInstance,
						  LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)),
						  LoadCursor(NULL, IDC_ARROW),
						  (HBRUSH)GetStockObject(BLACK_BRUSH),
						  NULL, wndclass };
	RegisterClass(&wndClass);

	DWORD wndStyle = WS_EX_TOPMOST;

	if (IsDebuggerPresent() || parent != NULL)
	{
		wndStyle = 0;
	}

	if (parent == NULL)
	{
		// Set the window's initial style
		m_dwWindowStyle = WS_BORDER | WS_DLGFRAME;

		// Set the window's initial width
		RECT rc;
		SetRect(&rc, 0, 0, 640, 480);
		AdjustWindowRect(&rc, (DWORD)m_dwWindowStyle, FALSE);
		FinalWidth = (rc.right - rc.left);
		FinalHeight = (rc.bottom - rc.top);
	}
	else
	{
		m_dwWindowStyle |= WS_CHILD;
	}

	// Create the render window
	m_hWnd = CreateWindowExA(wndStyle,
		wndclass, "X-Ray Oxygen", (DWORD)m_dwWindowStyle,
		/*rc.left, rc.top, */CW_USEDEFAULT, CW_USEDEFAULT,
		FinalWidth, FinalHeight, parent,
		0, hInstance, 0L);

	// Let the debugger know about game window
	gGameWindow = m_hWnd;

	return m_hWnd;
}

