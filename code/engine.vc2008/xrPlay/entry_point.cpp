////////////////////////////////////////
// OXYGEN TEAM, 2018 (C) * X-RAY OXYGEN	
// entry_point.cpp - entry point of xrPlay
// Edited: 13 May, 2018						
////////////////////////////////////////
#include <string>
#include <intrin.h>  
#include <windows.h>
#include <tlhelp32.h>
////////////////////////////////////
#include "../xrCore/xrCore.h"
////////////////////////////////////

void CreateRendererList();					// In RenderList.cpp

/// <summary> Dll import </summary>
using IsRunFunc = void(__cdecl*)(const char*);

/// <summary> Start engine or install OpenAL </summary>
void CheckOpenAL()
{
	CHAR szOpenALDir[MAX_PATH] = { 0 };
	R_ASSERT(GetSystemDirectoryA(szOpenALDir, MAX_PATH * sizeof(CHAR)));
	_snprintf_s(szOpenALDir, MAX_PATH * sizeof(CHAR), "%s%s", szOpenALDir, "\\OpenAL32.dll");

	DWORD dwOpenALInstalled = GetFileAttributesA(szOpenALDir);

	if (dwOpenALInstalled == INVALID_FILE_ATTRIBUTES)
	{
		xr_string StrCmd = xr_string(FS.get_path("$fs_root$")->m_Path) + "external\\oalinst.exe";

		dwOpenALInstalled = GetFileAttributesA(StrCmd.c_str());
		if (dwOpenALInstalled == INVALID_FILE_ATTRIBUTES)
		{
			xr_string szCmd = "/select, " + StrCmd + "\\fsgame.ltx";
			string_path szPath = { 0 };

			MessageBoxA(NULL,
				"ENG: X-Ray Oxygen can't detect OpenAL installer. Please, specify path to installer manually. \n"
				"RUS: X-Ray Oxygen не смог обнаружить установщик OpenAL. Пожалуйста, укажите путь до установщика самостоятельно.",
				"OpenAL Not Found!",
				MB_OK | MB_ICONERROR
			);

			OPENFILENAMEA oFN = {};
			// get params to our struct
			ZeroMemory(&oFN, sizeof(OPENFILENAMEA));
			oFN.lStructSize = sizeof(OPENFILENAMEA);
			oFN.hwndOwner = NULL;
			oFN.nMaxFile = MAX_PATH;
			oFN.lpstrFile = szPath;
			oFN.lpstrFilter = "(*.exe) Windows Executable\0*.exe\0";
			oFN.lpstrTitle = "Open file";
			oFN.lpstrFileTitle = NULL;
			oFN.lpstrInitialDir = NULL;
			oFN.nFilterIndex = 1;
			oFN.nMaxFileTitle = 0;
			oFN.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// if we can't open filedialog - exit
			if (!GetOpenFileNameA(&oFN))
			{
				ExitProcess(0);
			}
			
			StrCmd = szPath;
		}

		// create parent process with admin rights
		SHELLEXECUTEINFOA shellInfo = { sizeof(SHELLEXECUTEINFOA) };
		shellInfo.lpVerb = "runas";
		shellInfo.lpFile = StrCmd.c_str();
		shellInfo.lpParameters = nullptr;
		shellInfo.hwnd = NULL;
		shellInfo.nShow = SW_NORMAL;
		shellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

		// if user didn't press 'No' at Shell desktop notification
		if (ShellExecuteExA(&shellInfo))
		{
			WaitForSingleObject(shellInfo.hProcess, INFINITE);
		}

		if ((dwOpenALInstalled = GetFileAttributesA(szOpenALDir) == INVALID_FILE_ATTRIBUTES))
		{
			MessageBoxA(NULL,
				"ENG: X-Ray Oxygen can't detect OpenAL library. Please, re-install library manually. \n"
				"RUS: X-Ray Oxygen не смог обнаружить библиотеку OpenAL. Пожалуйста, переустановите библиотеку самостоятельно.",
				"OpenAL Not Found!",
				MB_OK | MB_ICONERROR
			);

			ExitProcess(0);
		}
	}
}

/// <summary> Main method for initialize xrEngine </summary>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    gModulesLoaded = true;
	LPCSTR params = lpCmdLine;

	try
	{
		// Init X-ray core
		Debug._initialize();
		Core._initialize("X-Ray Oxygen", nullptr, TRUE, "fsgame.ltx");
	}
	catch (...)
	{
		MessageBoxA(NULL, "Can't load xrCore!", "Init error", MB_OK | MB_ICONHAND);
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
			MessageBox(NULL, "It's can affect on the stability of the game.", "SSE3 isn't supported on your CPU", MB_OK | MB_ICONASTERISK);
			//#VERTVER: some part of vectors use SSE3 instructions
		}
		// Checking for AVX
#ifndef RELEASE_IA32
		else if (!CPU::Info.hasFeature(CPUFeature::AVX))
		{
			MessageBox(NULL, "It's can affect on the stability of the game.", "AVX isn't supported on your CPU!", MB_OK | MB_ICONWARNING);
		}
#endif
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
			NULL, FO_DELETE,
			tempdir, "",
			FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
			false, 0, ""
		};
		SHFileOperation(&file_op);
		free(tempdir); // Since we malloc-ed
	}

	params = lpCmdLine;

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
		HANDLE hProcess = NULL;
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
	if (hLib == NULL)
	{
		MessageBoxA(NULL, "Can't load xrEngine.dll!", "Init error", MB_OK | MB_ICONERROR);
		return 1;
	}

	IsRunFunc RunFunc = (IsRunFunc)GetProcAddress(hLib, "RunApplication");
	if (RunFunc)
	{
		RunFunc(params);
	}
	else
	{
		MessageBoxA(NULL, "xrEngine module doesn't seems to have RunApplication entry point. Different DLL?", "Init error", MB_OK | MB_ICONERROR);
		return 1;
	}
#ifdef NO_MULTI_INSTANCES		
	// Delete application presence mutex
	CloseHandle(hCheckPresenceMutex);
#endif
	return 0;
}