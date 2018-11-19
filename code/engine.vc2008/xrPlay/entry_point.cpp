////////////////////////////////////////
// OXYGEN TEAM, 2018 (C) * X-RAY OXYGEN	
// entry_point.cpp - entry point of xrPlay
// Edited: 13 May, 2018						
////////////////////////////////////////
#include <string>
#include <intrin.h>  
#include <windows.h>
////////////////////////////////////
#include "../xrCore/xrCore.h"
////////////////////////////////////

void CreateRendererList();					// In RenderList.cpp

/// <summary> Dll import </summary>
using IsRunFunc = void(__cdecl*)(const char*);

/// <summary> Start engine or install OpenAL </summary>
void CheckOpenAL(const char* params)
{
	DWORD dwOpenALInstalled = GetFileAttributes("C:\\Windows\\System32\\OpenAL32.dll");
	if (dwOpenALInstalled == INVALID_FILE_ATTRIBUTES)
	{
		xr_string StrCmd = "/select, " + xr_string(FS.get_path("$fs_root$")->m_Path) + "external\\oalinst.exe";
		StrCmd[11] = '\\';
		//WinExec(StrCmd.c_str(), 1);
		ShellExecute(NULL, NULL, "explorer.exe", StrCmd.c_str(), NULL, SW_SHOWNORMAL);
		system(StrCmd.c_str());
		MessageBox(0, "ENG: Click just after installing OpenAL. \n"
					  "RUS: Нажмите после установки OpenAL.", "OpenAL Not Found!", MB_OK);
	}
}

/// <summary> Main method for initialize xrEngine </summary>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	////////////////////////////////////////////////////
    gModulesLoaded = true;
	LPCSTR params = lpCmdLine;
	////////////////////////////////////////////////////
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

	////////////////////////////////////////////////////
	// If we don't needy for a exceptions - we can 
	// delete exceptions with option "-silent"
	////////////////////////////////////////////////////

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
	// Check for another instance
#ifdef NO_MULTI_INSTANCES
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
			CloseHandle(hCheckPresenceMutex);		// Already running
			return 1;
		}
	}
#endif
	CreateRendererList();
	CheckOpenAL(params);

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
		MessageBox(NULL, "xrEngine module doesn't seems to have RunApplication entry point. Different DLL?", "Init error", MB_OK | MB_ICONERROR);
		return 1;
	}
#ifdef NO_MULTI_INSTANCES		
	// Delete application presence mutex
	CloseHandle(hCheckPresenceMutex);
#endif
	return 0;
}