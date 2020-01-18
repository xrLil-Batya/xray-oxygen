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
		hEvent = CreateEventA(nullptr, FALSE, FALSE, FALSE);	// splash event
		R_ASSERT(hEvent);
		if (hEvent != NULL)
		{
			hThread = thread_spawn(WrapperSplashThreadProc, "Splash thread", 0, this);
			threadId = GetThreadId(hThread);
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
		PostThreadMessage(threadId, WM_QUIT, NULL, NULL);
		WaitForSingleObject(hThread, INFINITE);
		hThread = NULL;
		CloseHandle(hEvent); hEvent = NULL;
		delete pTask; pTask = nullptr;
	}
}

VOID DSplashScreen::SetSplashWindowName(xr_string windowName)
{
	splashWindowName = windowName;
}

VOID DSplashScreen::SetProgressPosition(DWORD percent, xr_string messageString)
{
	if (hThread)
	{
		pendingProgressMsg = messageString;
		PostThreadMessage(threadId, PBM_SETPOS, percent, NULL);
		pTask->SetValue(percent);
	}
}

VOID DSplashScreen::SetProgressColor(COLORREF refColor)
{
	barColor = refColor;
}

void WINAPI DSplashScreen::SplashThreadProc()
{
	Gdiplus::Image* pSplashImage = reinterpret_cast<Gdiplus::Image*>(pMainImage);

	// create splash window class
	WNDCLASSA windowsClass = { NULL };
	windowsClass.style = CS_HREDRAW | CS_VREDRAW;
	windowsClass.lpfnWndProc = WrapperSplashWndProc;
	windowsClass.hInstance = GetModuleHandleA(nullptr);	// process module
	windowsClass.hCursor = LoadCursorA(nullptr, IDC_APPSTARTING);
	windowsClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowsClass.lpszClassName = "OxySplashScreen";
	windowsClass.hIcon = LoadIcon(windowsClass.hInstance, MAKEINTRESOURCEA(IDI_ICON1));

	R_ASSERT(RegisterClass(&windowsClass));

	// try to find monitor where mouse was last time
	tagPOINT point = { NULL };
	tagMONITORINFO monitorInfo = { NULL };
	monitorInfo.cbSize = sizeof(tagMONITORINFO);
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
	hwndSplash = CreateWindowEx(splashWindowName.length() ? NULL : WS_EX_TOOLWINDOW,
		"OxySplashScreen", splashWindowName.c_str(), 
		WS_CLIPCHILDREN | WS_POPUP,
		areaRect.left, areaRect.top,
		pSplashImage->GetWidth(), pSplashImage->GetHeight(),
		hwndParent, nullptr, windowsClass.hInstance, nullptr
	);

	R_ASSERT(hwndSplash);
	if (!pTask) { pTask = new TaskbarValue(hwndSplash); }

	// set long pointer
	ShowWindow(hwndSplash, SW_SHOWNOACTIVATE);

	MSG msg = { nullptr };
	LONG timerCount = 0;
	SetEvent(hEvent);

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			if (msg.message == PBM_SETPOS)
			{
				KillTimer(nullptr, timerID);
				SendMessage(hwndSplash, PBM_SETPOS, msg.wParam, msg.lParam);
				continue;
			}

			if (msg.message == PBM_SETSTEP)
			{
				SendMessage(hwndSplash, PBM_SETPOS, LOWORD(msg.wParam), 0); // initiate progress bar creation
				SendMessage(hwndProgress, PBM_SETSTEP, (HIWORD(msg.wParam) - LOWORD(msg.wParam)) / msg.lParam, 0);
				timerCount = static_cast<LONG>(msg.lParam);
				timerID = SetTimer(nullptr, 0, 1000, nullptr);
				continue;
			}

			if (msg.message == WM_TIMER && msg.wParam == timerID)
			{
				SendMessage(hwndProgress, PBM_STEPIT, 0, 0);
				timerCount--;
				if (timerCount <= 0)
				{
					timerCount = 0;
					KillTimer(nullptr, timerID);
					Sleep(0);
				}
				continue;
			}

			if (msg.message == PBM_SETBARCOLOR)
			{
				if (!IsWindow(hwndProgress))
				{
					SendMessage(hwndSplash, PBM_SETPOS, 0, 0); // initiate progress bar creation
				}
				SendMessage(hwndProgress, PBM_SETBARCOLOR, msg.wParam, msg.lParam);
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

	if (hwndProgress != NULL)
	{
		DestroyWindow(hwndProgress);
		hwndProgress = NULL;
	}
	DestroyWindow(hwndSplash);
	hwndSplash = NULL;
}

LRESULT CALLBACK DSplashScreen::WrapperSplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return splashScreen.SplashWndProc(hwnd, uMsg, wParam, lParam);
}

void WINAPI DSplashScreen::WrapperSplashThreadProc(LPVOID pData)
{
	UNREFERENCED_PARAMETER(pData);
	splashScreen.SplashThreadProc();
}

LRESULT DSplashScreen::SplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Gdiplus::Image* pSplashImage = reinterpret_cast<Gdiplus::Image*>(pMainImage);

	switch (uMsg)
	{
	case WM_PAINT:
		if (pMainImage)
		{
			Gdiplus::Graphics gdip(hwnd);
			gdip.DrawImage(pSplashImage, 0, 0, pSplashImage->GetWidth(), pSplashImage->GetHeight());

			if (!progressMsg.empty())
			{
				// Форсер, заебал шрифт менять. Оставь хотя бы ебучий ариал, ей богу
				Gdiplus::Font msgFont(L"Arial", 18, Gdiplus::FontStyle::FontStyleRegular, Gdiplus::UnitPixel);

				Gdiplus::SolidBrush msgBrush(static_cast<DWORD>(Gdiplus::Color::White));
				//#VERTVER: PLS REWORK IT
				//////////////////////////////////////
				xr_string prgress = progressMsg.c_str();
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
		if (!IsWindow(hwndProgress))
		{
			RECT clientRect = { NULL };
			GetClientRect(hwnd, &clientRect);

			hwndProgress = CreateWindow(PROGRESS_CLASS, nullptr,
				WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
				4, clientRect.bottom - 20, clientRect.right - 8, 16,
				hwnd, nullptr, GetModuleHandle(nullptr), nullptr
			);

			SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
			SendMessage(hwndProgress, PBM_SETBKCOLOR, 0, RGB(0x21, 0x1D, 0x1E));
			PostThreadMessage(threadId, PBM_SETBARCOLOR, NULL, barColor);
		}
		SendMessage(hwndProgress, PBM_SETPOS, wParam, NULL);

		// if our message is not a previos 
		if (!pendingProgressMsg.empty() && progressMsg != pendingProgressMsg) // pInstance->progressMsg
		{
			progressMsg = pendingProgressMsg;
			SendMessage(hwndSplash, WM_PAINT, 0, 0);
		}

		return NULL;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
