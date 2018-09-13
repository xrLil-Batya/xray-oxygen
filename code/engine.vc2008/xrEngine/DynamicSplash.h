/*********************************************************
* Copyright (C) X-Ray Oxygen, 2018. All rights reserved.
* X-Ray Oxygen - open-source X-Ray fork
* Apache License
**********************************************************
* Module Name: Dynamic splash screen
**********************************************************
* DynamicSplash.h
* Methods for dynamic splash implementation
*********************************************************/
#pragma once

ATOM CreateSplashClass(HINSTANCE hInstance, LPCSTR lpClass, WNDPROC wndProc);

class DSplashScreen
{

public:
	DSplashScreen();
	DSplashScreen(HWND hwnd);
	DSplashScreen(const DSplashScreen&) {};
	DSplashScreen& operator=(const DSplashScreen&) {};
	~DSplashScreen() {};

	ICF HWND GetSplashWindow() { return hwndSplash; }
	VOID SetBackgroundImage(LPVOID pImage);
	VOID ShowSplash();
	VOID HideSplash();

	VOID SetSplashWindowName(xr_string windowName);
	VOID SetProgressPosition(DWORD percent);
	VOID SetProgressPosition(DWORD percent, xr_string messageString);
	VOID SetProgressPosition(DWORD percent, DWORD resourceId, HMODULE hModule);
	VOID SetProgressColor(COLORREF refColor);

protected:
	HANDLE hThread;
	DWORD threadId;
	HANDLE hEvent;

	LPVOID pMainImage;			
	xr_string splashWindowName;
	HWND hwndSplash;
	HWND hwndProgress;	
	HWND hwndParent;
	xr_string progressMsg;	
	UINT_PTR timerID;    

	static LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI SplashThreadProc(LPVOID pData);

};

VOID WINAPI InitSplash(HINSTANCE hInstance, LPCSTR lpClass, WNDPROC wndProc);