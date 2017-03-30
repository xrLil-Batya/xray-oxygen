// xrLauncher.cpp: определяет экспортированные функции для приложения DLL.
//
#include "stdafx.h"
#include <Windows.h>

LAUNCHAPI int RunXRLauncher()
{
	MessageBox(NULL, L"Aye", L"TExt", NULL);
	return 0;
}