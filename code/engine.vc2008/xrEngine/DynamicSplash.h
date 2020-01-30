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
		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		R_CHK(CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pTaskbar)));

		currentHwnd = hwnd;

		pTaskbar->SetProgressState(currentHwnd, TBPF_NORMAL);
		pTaskbar->SetProgressValue(currentHwnd, 0, 100);
	}

	void SetValue(DWORD Value)
	{
		if (Value == 100)
		{
			SetCompleted();
		}
		else pTaskbar->SetProgressValue(currentHwnd, Value, 100);
	}

	void SetError()
	{
		pTaskbar->SetProgressState(currentHwnd, TBPF_ERROR);
	}

	void SetPause()
	{
		pTaskbar->SetProgressState(currentHwnd, TBPF_PAUSED);
	}

	void SetCompleted()
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
	DSplashScreen& operator=(const DSplashScreen& a) { return *this; };

	void SetBackgroundImage(LPVOID pImage);
	void ShowSplash();
	void HideSplash();

	void SetSplashWindowName(xr_string windowName);
	void SetProgressPosition(DWORD percent, xr_string messageString);
	void SetProgressColor(COLORREF refColor);

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
	xr_string pendingProgressMsg;
	UINT_PTR timerID;    
	COLORREF barColor;

	LRESULT SplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WrapperSplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void WINAPI WrapperSplashThreadProc(LPVOID pData);
	void WINAPI SplashThreadProc();
};

void WINAPI InitSplash(HINSTANCE hInstance, LPCSTR lpClass, WNDPROC wndProc);
