////////////////////////////////////////
// OXYGEN TEAM, 2018 (C) * X-RAY OXYGEN	
// entry_point.cpp - entry point of xrPlay
// Edited: 13 May, 2018						
////////////////////////////////////////
#include <string>
#include <intrin.h>  
#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
////////////////////////////////////
#include "../xrCore/xrCore.h"

void CreateRendererList();					// In RenderList.cpp

/// <summary> Dll import </summary>
using IsRunFunc = void(__cdecl*)(const char*);

/// <summary> Start engine or install OpenAL </summary>
void CheckOpenAL();

/// <summary> Main method for initialize xrEngine </summary>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int)
{
    gModulesLoaded = true;

	try
	{
		// Init X-ray core
		Debug._initialize();
		Core._initialize("X-Ray Oxygen", nullptr, TRUE, "fsgame.ltx");
	}
	catch (...)
	{
		MessageBoxA(nullptr, "Can't load xrCore!", "Init error", MB_OK | MB_ICONHAND);
	}

#ifndef DEBUG
	if (!strstr(lpCmdLine, "-silent"))
	{
		// Checking for SSE2
		if (!CPU::Info.hasFeature(CPUFeature::SSE2))
		{
			return 0;
		}
		// Checking for SSE3
		else if (!CPU::Info.hasFeature(CPUFeature::SSE3))
		{
			MessageBox(nullptr, "It can affect on the stability of the game.", "SSE3 isn't supported on your CPU", MB_OK | MB_ICONASTERISK);
			//#VERTVER: some part of vectors use SSE3 instructions
		}
	}
#endif

	// Might be useful for shader developers. No need to clear shaders cache folder each time.
	if (strstr(Core.Params, "-delete_shaderscache"))
	{
		xr_string ShadersCacheFolderPath;
		ShadersCacheFolderPath = FS.get_path("$app_data_root$")->m_Path;
		ShadersCacheFolderPath.append("shaders_cache\\*");

		int len = strlen(ShadersCacheFolderPath.c_str()) + 2; // required to set 2 nulls at end of argument to SHFileOperation.
		char* tempdir = (char*)malloc(len);
		memset(tempdir, 0, len);
		strcpy(tempdir, ShadersCacheFolderPath.c_str());

		SHFILEOPSTRUCT file_op = 
		{
			nullptr, FO_DELETE,
			tempdir, "",
			FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
			false, nullptr, ""
		};

		SHFileOperation(&file_op);
		free(tempdir); // Since we malloc-ed
	}

	if (!IsDebuggerPresent())
	{
		size_t HeapFragValue = 2;
		HeapSetInformation(GetProcessHeap(), HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue));
	}

	CreateRendererList();
	CheckOpenAL();

	if (!strstr(Core.Params, "-unlimited_game_instances"))
	{
#ifndef OLD_INSTANCE_SYSTEM
		PROCESSENTRY32W processInfo = { NULL };
		processInfo.dwSize = sizeof(PROCESSENTRY32W);
		DWORD CurrentProcessId = GetCurrentProcessId();
		DWORD CustomProcessId = 0;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		HANDLE hProcess = nullptr;
		WCHAR szBuf[MAX_PATH] = { NULL };
		BOOL bSearch = Process32FirstW(hSnapshot, &processInfo);
		BOOL isReady = FALSE;

		while (bSearch)
		{
			//#NOTE: it's only game module: no editor or other module
			int iCompareString = wcsncmp(L"xrPlay.exe", processInfo.szExeFile, MAX_PATH);

			// we found our second process. Show our parent process window and terminate this.
			if (!iCompareString && processInfo.th32ProcessID != CurrentProcessId)
			{
				HWND hWindow = FindWindowA("_XRAY_1.7", "X-Ray Oxygen");

				if (hWindow)
				{
					ShowWindow(hWindow, 0);
				}

				ExitProcess(0x2);
			}

			// search next process
			bSearch = Process32NextW(hSnapshot, &processInfo);
		}

		CloseHandle(hSnapshot);
#else
		constexpr char* STALKER_PRESENCE_MUTEX = "Local\\STALKER-COP";
		HANDLE hCheckPresenceMutex = OpenMutex(READ_CONTROL, FALSE, STALKER_PRESENCE_MUTEX);
		if (!strstr(lpCmdLine, "-editor"))
		{
			if (hCheckPresenceMutex == NULL)
			{
				hCheckPresenceMutex = CreateMutex(NULL, FALSE, STALKER_PRESENCE_MUTEX);	// New mutex
				if (hCheckPresenceMutex == NULL)
					return 2;
			}
			else
			{
				HWND hWindow = FindWindowA("_XRAY_1.7", "X-Ray Oxygen");

				if (hWindow)
				{
					ShowWindow(hWindow, 0);
				}

				CloseHandle(hCheckPresenceMutex);		// Already running
				return 1;
			}
		}
#endif
	}

	HMODULE hLib = LoadLibrary("xrEngine.dll");
	if (hLib == nullptr)
	{
		MessageBoxA(nullptr, "Cannot load xrEngine.dll!", "X-Ray Oxygen - Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	IsRunFunc RunFunc = reinterpret_cast<IsRunFunc>(GetProcAddress(hLib, "RunApplication"));
	if (RunFunc)
	{
		RunFunc(lpCmdLine);
	}
	else
	{
		MessageBoxA(nullptr, "xrEngine module doesn't seem to have RunApplication entry point. Different DLL?", "Init error", MB_OK | MB_ICONERROR);
		return 1;
	}
#ifdef OLD_INSTANCE_SYSTEM		
	// Delete application presence mutex
	CloseHandle(hCheckPresenceMutex);
#endif
	gModulesLoaded = false;
	return 0;
}
