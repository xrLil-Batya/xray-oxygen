#include "../xrCore/xrCore.h"
////////////////////////////////////
#include <tlhelp32.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

bool IsProcessWithAdminPrivilege()
{
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	LPVOID pAdministratorsGroup = nullptr;
	BOOL bRet = FALSE;

	// init SID to control privileges
	AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdministratorsGroup);

	// ckeck membership
	CheckTokenMembership(nullptr, pAdministratorsGroup, &bRet);

	// clean pointer
	if (pAdministratorsGroup) { FreeSid(pAdministratorsGroup); pAdministratorsGroup = nullptr; }

	return !!bRet;
}

void CheckOpenAL()
{
#if 0
	TCHAR szOpenALDir[MAX_PATH] = { 0 };
	R_ASSERT(GetSystemDirectory(szOpenALDir, MAX_PATH * sizeof(TCHAR)));

#ifndef UNICODE 
	_snprintf_s(szOpenALDir, MAX_PATH * sizeof(CHAR), "%s%s", szOpenALDir, "\\OpenAL32.dll");
#else
	_snwprintf_s(szOpenALDir, MAX_PATH * sizeof(WCHAR), L"%s%s", szOpenALDir, L"\\OpenAL32.dll");
#endif
	DWORD dwOpenALInstalled = GetFileAttributes(szOpenALDir);

	if (dwOpenALInstalled == INVALID_FILE_ATTRIBUTES)
	{

#ifndef UNICODE
		xr_string StrCmd = xr_string(FS.get_path("$fs_root$")->m_Path) + "external\\oalinst.exe";
#else
		std::wstring StrCmd = std::wstring(FS.get_path("$fs_root$")->m_Path) + L"external\\oalinst.exe"''
#endif

		// if current user is admin - go to run this application
		if (!IsProcessWithAdminPrivilege())
		{
			TCHAR szPathToLib[MAX_PATH] = { 0 };
			GetModuleFileName(nullptr, szPathToLib, ARRAYSIZE(szPathToLib));

			SHELLEXECUTEINFO shellInfo = { sizeof(SHELLEXECUTEINFO) };
			shellInfo.lpVerb = TEXT("runas");
			shellInfo.lpFile = szPathToLib;
			shellInfo.hwnd = nullptr;
			shellInfo.nShow = SW_NORMAL;

			if (ShellExecuteEx(&shellInfo)) { ExitProcess(GetCurrentProcessId()); }
		}

		TCHAR szPath[MAX_PATH] = { 0 };
		GetModuleFileName(GetModuleHandle(nullptr), szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);

#ifndef UNICODE 
		_snprintf_s(szPath, MAX_PATH * sizeof(CHAR), "%s%s", szPath, "\\OpenAL32.dll");
#else
		_snwprintf_s(szPath, MAX_PATH * sizeof(WCHAR), L"%s%s", szPath, L"\\OpenAL32.dll");
#endif

		dwOpenALInstalled = GetFileAttributes(szPath);
		if (dwOpenALInstalled == INVALID_FILE_ATTRIBUTES)
		{
			MessageBoxW(nullptr,
				L"ENG: X-Ray Oxygen can't detect OpenAL library. Please, specify path to installer manually. \n"
				L"RUS: X-Ray Oxygen не смог обнаружить библиотеку OpenAL. Пожалуйста, укажите путь до установщика самостоятельно.",
				L"OpenAL Not Found!",
				MB_OK | MB_ICONERROR
			);

			OPENFILENAME oFN = { 0 };
			oFN.lStructSize = sizeof(OPENFILENAME);
			oFN.hwndOwner = nullptr;
			oFN.nMaxFile = MAX_PATH;
			oFN.lpstrFile = szPath;
			oFN.lpstrFilter = TEXT("(*.exe) Windows Executable\0*.exe\0");
			oFN.lpstrTitle = TEXT("Open file");
			oFN.lpstrFileTitle = nullptr;
			oFN.lpstrInitialDir = nullptr;
			oFN.nFilterIndex = 1;
			oFN.nMaxFileTitle = 0;
			oFN.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// if we can't open filedialog - exit
			if (!GetOpenFileName(&oFN))
			{
				ExitProcess(0);
			}

			StrCmd = szPath;

			// create parent process with admin rights
			SHELLEXECUTEINFO shellInfo = { sizeof(SHELLEXECUTEINFO) };
			shellInfo.lpVerb = TEXT("runas");
			shellInfo.lpFile = StrCmd.c_str();
			shellInfo.lpParameters = nullptr;
			shellInfo.hwnd = nullptr;
			shellInfo.nShow = SW_NORMAL;
			shellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

			// if user didn't press 'No' at Shell desktop notification
			if (ShellExecuteEx(&shellInfo))
			{
				WaitForSingleObject(shellInfo.hProcess, INFINITE);
			}
		}
		else
		{
			DWORD LibrarySize = 0;
			HANDLE hFile = CreateFile(szPath, GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			R_ASSERT(hFile != INVALID_HANDLE_VALUE);

			FILE_STANDARD_INFO fileInfo = { 0 };
			GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo));

			LPVOID pImage = HeapAlloc(GetProcessHeap(), 0, fileInfo.EndOfFile.QuadPart);
			ReadFile(hFile, pImage, fileInfo.EndOfFile.QuadPart, &LibrarySize, nullptr);

			CloseHandle(hFile);

			hFile = CreateFile(szOpenALDir, GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			R_ASSERT(hFile != INVALID_HANDLE_VALUE);

			WriteFile(hFile, pImage, fileInfo.EndOfFile.QuadPart, &LibrarySize, nullptr);

			HeapFree(GetProcessHeap(), 0, pImage);
			CloseHandle(hFile);
		}

		if ((dwOpenALInstalled = GetFileAttributes(szOpenALDir) == INVALID_FILE_ATTRIBUTES))
		{
			MessageBoxW(nullptr,
				L"ENG: X-Ray Oxygen can't detect OpenAL library. Please, re-install library manually. \n"
				L"RUS: X-Ray Oxygen не смог обнаружить библиотеку OpenAL. Пожалуйста, переустановите библиотеку самостоятельно.",
				L"OpenAL Not Found!",
				MB_OK | MB_ICONERROR
			);

			ExitProcess(0);
		}
	}
#endif
}
