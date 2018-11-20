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

#include <Shobjidl.h>

class ENGINE_API TaskbarValue
{
#ifndef _RELEASE1
#define _RELEASE1(X) if (X) { X->Release(); X = NULL; }
#endif
public:
	TaskbarValue(HWND hwnd)
	{
		// if failed - OS is lower then Windows 7, because TaskbarList3 is not exist at Vista
		R_CHK(CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pTaskbar)));

		currentHwnd = hwnd;

		pTaskbar->SetProgressState(currentHwnd, TBPF_NORMAL);
		pTaskbar->SetProgressValue(currentHwnd, 0, 100);
	}

	VOID SetValue(DWORD Value)
	{
		if (Value == 100)
		{
			pTaskbar->SetProgressState(currentHwnd, TBPF_NOPROGRESS);
		}

		pTaskbar->SetProgressValue(currentHwnd, Value, 100);
	}

	VOID SetError()
	{
		pTaskbar->SetProgressState(currentHwnd, TBPF_ERROR);
	}

	VOID SetPause()
	{
		pTaskbar->SetProgressState(currentHwnd, TBPF_PAUSED);
	}

	VOID SetCompleted()
	{
		pTaskbar->SetProgressState(currentHwnd, TBPF_NOPROGRESS);
	}

	~TaskbarValue()
	{
		_RELEASE1(pTaskbar);
	}

private:
	HWND currentHwnd;
	ITaskbarList3* pTaskbar;
#undef _RELEASE1
};


ATOM CreateSplashClass(HINSTANCE hInstance, LPCSTR lpClass, WNDPROC wndProc);

class ENGINE_API DSplashScreen
{
public:
	DSplashScreen();
	DSplashScreen(HWND hwnd);
	DSplashScreen(const DSplashScreen&) {};
	DSplashScreen& operator=(const DSplashScreen& a) { return *this; };
	~DSplashScreen() { delete pTask; };

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

	TaskbarValue* pTask;
	LPVOID pMainImage;			
	xr_string splashWindowName;
	HWND hwndSplash;
	HWND hwndProgress;	
	HWND hwndParent;
	xr_string progressMsg;	
	UINT_PTR timerID;    

	static LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static UINT WINAPI SplashThreadProc(LPVOID pData);
};

VOID WINAPI InitSplash(HINSTANCE hInstance, LPCSTR lpClass, WNDPROC wndProc);
