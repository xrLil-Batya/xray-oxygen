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
	CHAR szOpenALDir[MAX_PATH] = { 0 };
	R_ASSERT(GetSystemDirectory(szOpenALDir, MAX_PATH * sizeof(CHAR)));
	_snprintf_s(szOpenALDir, MAX_PATH * sizeof(CHAR), "%s%s", szOpenALDir, "\\OpenAL32.dll");

	DWORD dwOpenALInstalled = GetFileAttributes(szOpenALDir);

	if (dwOpenALInstalled == INVALID_FILE_ATTRIBUTES)
	{
		xr_string StrCmd = xr_string(FS.get_path("$fs_root$")->m_Path) + "external\\oalinst.exe";

		string_path szPath = { 0 };

		// if current user is admin - go to run this application
		if (!IsProcessWithAdminPrivilege())
		{
			string_path szPath = { 0 };
			GetModuleFileName(nullptr, szPath, ARRAYSIZE(szPath));

			SHELLEXECUTEINFOA shellInfo = { sizeof(SHELLEXECUTEINFOA) };
			shellInfo.lpVerb = "runas";
			shellInfo.lpFile = szPath;
			shellInfo.hwnd = nullptr;
			shellInfo.nShow = SW_NORMAL;

			if (ShellExecuteEx(&shellInfo)) { ExitProcess(GetCurrentProcessId()); }
		}

		GetModuleFileName(GetModuleHandleA(nullptr), szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);

		_snprintf_s(szPath, MAX_PATH * sizeof(CHAR), "%s%s", szPath, "\\OpenAL32.dll");

		dwOpenALInstalled = GetFileAttributesA(szPath);
		if (dwOpenALInstalled == INVALID_FILE_ATTRIBUTES)
		{
			MessageBox(nullptr,
				"ENG: X-Ray Oxygen can't detect OpenAL library. Please, specify path to installer manually. \n"
				"RUS: X-Ray Oxygen не смог обнаружить библиотеку OpenAL. Пожалуйста, укажите путь до установщика самостоятельно.",
				"OpenAL Not Found!",
				MB_OK | MB_ICONERROR
			);

			OPENFILENAMEA oFN = {};
			// get params to our struct
			ZeroMemory(&oFN, sizeof(OPENFILENAMEA));
			oFN.lStructSize = sizeof(OPENFILENAMEA);
			oFN.hwndOwner = nullptr;
			oFN.nMaxFile = MAX_PATH;
			oFN.lpstrFile = szPath;
			oFN.lpstrFilter = "(*.exe) Windows Executable\0*.exe\0";
			oFN.lpstrTitle = "Open file";
			oFN.lpstrFileTitle = nullptr;
			oFN.lpstrInitialDir = nullptr;
			oFN.nFilterIndex = 1;
			oFN.nMaxFileTitle = 0;
			oFN.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// if we can't open filedialog - exit
			if (!GetOpenFileNameA(&oFN))
			{
				ExitProcess(0);
			}

			//StrCmd = szPath;

			// create parent process with admin rights
			SHELLEXECUTEINFOA shellInfo = { sizeof(SHELLEXECUTEINFOA) };
			shellInfo.lpVerb = "runas";
			shellInfo.lpFile = StrCmd.c_str();
			shellInfo.lpParameters = nullptr;
			shellInfo.hwnd = nullptr;
			shellInfo.nShow = SW_NORMAL;
			shellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

			// if user didn't press 'No' at Shell desktop notification
			if (ShellExecuteExA(&shellInfo))
			{
				WaitForSingleObject(shellInfo.hProcess, INFINITE);
			}
		}
		else
		{
			DWORD LibrarySize = 0;
			HANDLE hFile = CreateFile(szPath, GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			R_ASSERT(hFile != INVALID_HANDLE_VALUE);

			FILE_STANDARD_INFO fileInfo = {};
			GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo));

			LPVOID pImage = HeapAlloc(GetProcessHeap(), NULL, fileInfo.EndOfFile.QuadPart);
			ReadFile(hFile, pImage, fileInfo.EndOfFile.QuadPart, &LibrarySize, nullptr);

			CloseHandle(hFile);

			//szOpenALDir
			hFile = CreateFile(szOpenALDir, GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			R_ASSERT(hFile != INVALID_HANDLE_VALUE);

			WriteFile(hFile, pImage, fileInfo.EndOfFile.QuadPart, &LibrarySize, nullptr);
			CloseHandle(hFile);
		}

		if ((dwOpenALInstalled = GetFileAttributes(szOpenALDir) == INVALID_FILE_ATTRIBUTES))
		{
			MessageBox(nullptr,
				"ENG: X-Ray Oxygen can't detect OpenAL library. Please, re-install library manually. \n"
				"RUS: X-Ray Oxygen не смог обнаружить библиотеку OpenAL. Пожалуйста, переустановите библиотеку самостоятельно.",
				"OpenAL Not Found!",
				MB_OK | MB_ICONERROR
			);

			ExitProcess(0);
		}
	}
}
