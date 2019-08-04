/*********************************************************
* Copyright (C) X-Ray Oxygen, 2018. All rights reserved.
* X-Ray Oxygen - open-source X-Ray fork
* Apache License
**********************************************************
* Module Name: Dynamic splash screen
**********************************************************
* DynamicSplash.cpp
* Methods for dynamic splash implementation
*********************************************************/
#include "stdafx.h"
#include "DynamicSplash.h"

#include <process.h>
#include <CommCtrl.h>
#include <WinUser.h>
#include "../xrPlay/resource.h"

#pragma warning(push)
#pragma warning(disable: 4458)
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#include <gdiplus.h>
#include <gdiplusinit.h>
#pragma warning(pop)

ENGINE_API DSplashScreen splashScreen;

ATOM CreateSplashClass(HINSTANCE hInstance, LPCSTR lpClass, WNDPROC wndProc)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(nullptr, IDC_APPSTARTING);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)BLACK_BRUSH;
	wcex.lpszMenuName = MAKEINTRESOURCE(IDB_BITMAP1);
	wcex.lpszClassName = lpClass;
	wcex.hIconSm = LoadIcon(nullptr, IDC_APPSTARTING);

	return RegisterClassEx(&wcex);
}

VOID WINAPI InitSplash(HINSTANCE hInstance, LPCSTR lpClass, WNDPROC wndProc)
{
	CreateSplashClass(hInstance, lpClass, wndProc);

	Gdiplus::GdiplusStartupInput gdiSI;
	Gdiplus::GdiplusStartupOutput gdiSO;
	ULONG_PTR gdiToken;
	ULONG_PTR gdiHookToken;
	gdiSI.SuppressBackgroundThread = TRUE;
	Gdiplus::GdiplusStartup(&gdiToken, &gdiSI, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);

	//#VERTVER: PLS REWORK IT
	//////////////////////////////////////
	FS_Path* filePath = FS.get_path("$textures$");
	xr_string szPath = xr_string(filePath->m_Path);
	std::wstring szWPath = std::wstring(szPath.begin(), szPath.end());
	szWPath += L"ui\\Splash.bmp";
	//////////////////////////////////////

	Gdiplus::Image* pImage = Gdiplus::Image::FromFile(szWPath.c_str());
	R_ASSERT(pImage);

	splashScreen.SetBackgroundImage(pImage);
	splashScreen.SetSplashWindowName("Oxy splash");
	delete pImage;

	splashScreen.ShowSplash();
	splashScreen.SetProgressPosition(0, "Engine entry-point");
}

DSplashScreen::DSplashScreen(HWND hwnd)
{
	pTask = new TaskbarValue(hwnd);

	hThread = nullptr;
	pMainImage = nullptr;
	hwndSplash = nullptr;
	threadId = NULL;
	hwndProgress = nullptr;
	hEvent = nullptr;
	hwndParent = hwnd;
}

DSplashScreen::DSplashScreen()
{
	hThread = nullptr;
	pMainImage = nullptr;
	hwndSplash = nullptr;
	threadId = NULL;
	hwndProgress = nullptr;
	hEvent = nullptr;
	hwndParent = nullptr;
}

VOID DSplashScreen::SetBackgroundImage(LPVOID pImage)
{
	if (pImage)
	{
		Gdiplus::Image* pCustomImage = reinterpret_cast<Gdiplus::Image*>(pImage);
		pMainImage = pCustomImage->Clone();
	}
}

VOID DSplashScreen::ShowSplash()
{
	if (!hThread)
	{
		// create splash thread
		UINT threadID = 0;
		hEvent = CreateEventA(nullptr, FALSE, FALSE, FALSE);	// splash event
		R_ASSERT(hEvent);
		if (hEvent != NULL)
		{
			hThread = (HANDLE)_beginthreadex(nullptr, 0, SplashThreadProc, static_cast<LPVOID>(this), 0, &threadID);
			threadId = threadID;
			R_ASSERT(WaitForSingleObject(hEvent, 5000) != WAIT_TIMEOUT);
		}
	}
	else
	{
		PostThreadMessage(threadId, WM_ACTIVATE, WA_CLICKACTIVE, NULL);
	}
}

VOID DSplashScreen::HideSplash()
{
	if (hThread)
	{
		TerminateThread(hThread, (DWORD)WAIT_TIMEOUT);
		CloseHandle(hThread);
		CloseHandle(hEvent);
	}
	hThread = nullptr;
}

VOID DSplashScreen::SetSplashWindowName(xr_string windowName)
{
	splashWindowName = windowName;
}

VOID DSplashScreen::SetProgressPosition(DWORD percent, xr_string messageString)
{
	if (hThread)
	{
		PostThreadMessage(threadId, PBM_SETPOS, percent, reinterpret_cast<LPARAM>(&messageString));
		pTask->SetValue(percent);
	}
}

VOID DSplashScreen::SetProgressPosition(DWORD percent, DWORD resourceId, HMODULE hModule)
{
	LPSTR lpMessage = nullptr;
	int len = ::LoadStringA(hModule, resourceId, reinterpret_cast<LPSTR>(&lpMessage), NULL);

	xr_string* tempMsg = new xr_string(lpMessage, len);
	PostThreadMessage(threadId, PBM_SETPOS, percent, reinterpret_cast<LPARAM>(tempMsg));
	pTask->SetValue(percent);
}

VOID DSplashScreen::SetProgressColor(COLORREF refColor)
{
	PostThreadMessage(threadId, PBM_SETBARCOLOR, NULL, refColor);
}

UINT WINAPI DSplashScreen::SplashThreadProc(LPVOID pData)
{
	string128 SplashScreenThreadName = "X-RAY Splashscreen thread";
	PlatformUtils.SetCurrentThreadName(SplashScreenThreadName);

	DSplashScreen* pSplash = static_cast<DSplashScreen*>(pData);
	if (!pSplash) { return 0; }

	Gdiplus::Image* pSplashImage = reinterpret_cast<Gdiplus::Image*>(pSplash->pMainImage);

	// create splash window class
	WNDCLASSA windowsClass = { NULL };
	windowsClass.style = CS_HREDRAW | CS_VREDRAW;
	windowsClass.lpfnWndProc = SplashWndProc;
	windowsClass.hInstance = GetModuleHandleA(nullptr);	// process module
	windowsClass.hCursor = LoadCursorA(nullptr, IDC_APPSTARTING);
	windowsClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowsClass.lpszClassName = "OxySplashScreen";
	windowsClass.hIcon = LoadIcon(windowsClass.hInstance, MAKEINTRESOURCEA(IDI_ICON1));

	R_ASSERT(RegisterClass(&windowsClass));

	// try to find monitor where mouse was last time
	tagPOINT point = { NULL };
	tagMONITORINFO monitorInfo = { NULL };
	monitorInfo.cbSize = sizeof(MONITORINFOEXA);
	HMONITOR hMonitor = nullptr;
	RECT areaRect = { NULL };

	GetCursorPos(&point);
	hMonitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);

	// get window info to rect
	if (GetMonitorInfo(hMonitor, &monitorInfo))
	{
		areaRect.left = (monitorInfo.rcMonitor.right + monitorInfo.rcMonitor.left - static_cast<LONG>(pSplashImage->GetWidth())) / 2;
		areaRect.top = (monitorInfo.rcMonitor.top + monitorInfo.rcMonitor.bottom - static_cast<LONG>(pSplashImage->GetHeight())) / 2;
	}
	else
	{
		SystemParametersInfoA(SPI_GETWORKAREA, NULL, &areaRect, NULL);
		areaRect.left = (areaRect.right + areaRect.left - pSplashImage->GetWidth()) / 2;
		areaRect.top = (areaRect.top + areaRect.bottom - pSplashImage->GetHeight()) / 2;
	}

	// create splash window
	pSplash->hwndSplash = CreateWindowEx(pSplash->splashWindowName.length() ? NULL : WS_EX_TOOLWINDOW,
		"OxySplashScreen", pSplash->splashWindowName.c_str(), 
		WS_CLIPCHILDREN | WS_POPUP,
		areaRect.left, areaRect.top,
		pSplashImage->GetWidth(), pSplashImage->GetHeight(),
		pSplash->hwndParent, nullptr, windowsClass.hInstance, nullptr
	);

	R_ASSERT(pSplash->hwndSplash);
	if (!pSplash->pTask) { pSplash->pTask = new TaskbarValue(pSplash->hwndSplash); }

	// set long pointer
	SetWindowLongPtr(pSplash->hwndSplash, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pSplash));
	ShowWindow(pSplash->hwndSplash, SW_SHOWNOACTIVATE);

	MSG msg = { nullptr };
	LONG timerCount = 0;
	SetEvent(pSplash->hEvent);

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			if (msg.message == PBM_SETPOS)
			{
				KillTimer(nullptr, pSplash->timerID);
				SendMessage(pSplash->hwndSplash, PBM_SETPOS, msg.wParam, msg.lParam);
				continue;
			}

			if (msg.message == PBM_SETSTEP)
			{
				SendMessage(pSplash->hwndSplash, PBM_SETPOS, LOWORD(msg.wParam), 0); // initiate progress bar creation
				SendMessage(pSplash->hwndProgress, PBM_SETSTEP, (HIWORD(msg.wParam) - LOWORD(msg.wParam)) / msg.lParam, 0);
				timerCount = static_cast<LONG>(msg.lParam);
				pSplash->timerID = SetTimer(nullptr, 0, 1000, nullptr);
				continue;
			}

			if (msg.message == WM_TIMER && msg.wParam == pSplash->timerID)
			{
				SendMessage(pSplash->hwndProgress, PBM_STEPIT, 0, 0);
				timerCount--;
				if (timerCount <= 0)
				{
					timerCount = 0;
					KillTimer(nullptr, pSplash->timerID);
					Sleep(0);
				}
				continue;
			}

			if (msg.message == PBM_SETBARCOLOR)
			{
				if (!IsWindow(pSplash->hwndProgress))
				{
					SendMessage(pSplash->hwndSplash, PBM_SETPOS, 0, 0); // initiate progress bar creation
				}
				SendMessage(pSplash->hwndProgress, PBM_SETBARCOLOR, msg.wParam, msg.lParam);
				continue;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Sleep(5);
		}
	}

	DestroyWindow(pSplash->hwndSplash);
	return NULL;
}

LRESULT CALLBACK DSplashScreen::SplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DSplashScreen* pInstance = reinterpret_cast<DSplashScreen*>(GetWindowLongPtr(hwnd, (-21)));
	if (!pInstance) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
	
	Gdiplus::Image* pSplashImage = reinterpret_cast<Gdiplus::Image*>(pInstance->pMainImage);

	switch (uMsg)
	{
	case WM_PAINT:
		if (pInstance->pMainImage)
		{
			Gdiplus::Graphics gdip(hwnd);
			gdip.DrawImage(pSplashImage, 0, 0, pSplashImage->GetWidth(), pSplashImage->GetHeight());

			if (!pInstance->progressMsg.empty())
			{
				// Форсер, заебал шрифт менять. Оставь хотя бы ебучий ариал, ей богу
				Gdiplus::Font msgFont(L"Arial", 18, Gdiplus::FontStyle::FontStyleRegular, Gdiplus::UnitPixel);

				Gdiplus::SolidBrush msgBrush(static_cast<DWORD>(Gdiplus::Color::White));
				//#VERTVER: PLS REWORK IT
				//////////////////////////////////////
				xr_string prgress = pInstance->progressMsg.c_str();
				std::wstring progressName(prgress.begin(), prgress.end());
				//////////////////////////////////////

				gdip.DrawString(progressName.c_str(), -1, &msgFont, Gdiplus::PointF(3.0f, pSplashImage->GetHeight() - 45.0f), &msgBrush);
			}
			ValidateRect(hwnd, nullptr);
			return 0;
		}
		break;  
	case PBM_SETPOS:
		// if window is not be created - create it 
		if (!IsWindow(pInstance->hwndProgress))
		{
			RECT clientRect = { NULL };
			GetClientRect(hwnd, &clientRect);

			pInstance->hwndProgress = CreateWindow(PROGRESS_CLASS, nullptr,
				WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
				4, clientRect.bottom - 20, clientRect.right - 8, 16,
				hwnd, nullptr, GetModuleHandle(nullptr), nullptr
			);

			SendMessage(pInstance->hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
			SendMessage(pInstance->hwndProgress, PBM_SETBKCOLOR, 0, RGB(0x21, 0x1D, 0x1E));
		}
		SendMessage(pInstance->hwndProgress, PBM_SETPOS, wParam, NULL);

		xr_string* msgThread = reinterpret_cast<xr_string*>(lParam);
		if (!msgThread || msgThread->size() > 1000000)
			return 0;

		// if our message is not a previos 
		if (!msgThread->empty() && pInstance->progressMsg != *msgThread) // pInstance->progressMsg
		{
			pInstance->progressMsg = *msgThread;
			SendMessage(pInstance->hwndSplash, WM_PAINT, 0, 0);
		}

		return NULL;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
