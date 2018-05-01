/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 30 April, 2018
* xrFS.cpp - filesystem for launcher
* FileSystem
*************************************************/
/////////////////////////////////////////
#pragma once
/////////////////////////////////////////
#include "xrFS.h"
/////////////////////////////////////////
#ifdef __cplusplus
/////////////////////////////////////////
QSettings qs("OxyTeam", "xrDevLaunch");
_TCHAR szPath[] = _T("Software\\xrDevLaunch\\");
_TCHAR szBuf[MAX_PATH];
DWORD dwBufLen = MAX_PATH;
HKEY hKey;
/////////////////////////////////////////


void FileSystem::createGlobalQtSetting(LPCSTR szCompany, LPCSTR szName) 
{
	QSettings settings(szCompany, szName);
}


void FileSystem::createAllLocalQtSettings()
{
	/////////////////////////////////////////
	qs.setValue				("main/default", 1);
	qs.setValue				("main/regeditSettings", 0);				// It's not recommended to use
	qs.setValue				("main/statusBarWithoutCPUID", 0);
	/////////////////////////////////////////
	qs.setValue				("main/startGameWithoutLauncher", 0);
	qs.setValue				("main/coreInit", 0);
	qs.setValue				("main/startMode", 0);					// Normal
	//qs.setValue			("main/startMode", 1);					// Minimized
	//qs.setValue			("main/startMode", 2);					// Maximized
	/////////////////////////////////////////
	qs.setValue				("main/awda", 0);
	/////////////////////////////////////////
	qs.setValue				("main/ecxeptionsOff", 0);
	qs.setValue				("main/messageBoxExceptions", 0);
	qs.setValue				("main/statusBarExceptions", 0);
	/////////////////////////////////////////
}


int FileSystem::getValueSetting(LPCSTR szPath)
{
	int value				= qs.value(szPath).toInt();
	return value;
}


void FileSystem::setValueSetting(LPCSTR szPath, int value)
{
	qs.setValue				(szPath, value);
}


void FileSystem::createReg	(_TCHAR szString[],
							 _TCHAR szValue[])
{
	/////////////////////////////////////////
	//#VERTVER: for validating regedit 
	//record, we need to use ERROR_SUCCESS
	//key. That's not fastest method, but
	//more safety.
	/////////////////////////////////////////
	_tsetlocale				(LC_ALL, _T( "English" ));
	/////////////////////////////////////////
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szPath, 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
		MessageBox(NULL, "1","All methods are ready!", MB_OK | MB_ICONINFORMATION);
	if (RegSetValueEx(hKey, _T(szString), 0, REG_SZ, (BYTE*)szValue, sizeof(szValue)) != ERROR_SUCCESS) 
		MessageBox(NULL, "2", "All methods are ready!", MB_OK | MB_ICONINFORMATION);
	if (RegCloseKey(hKey) != ERROR_SUCCESS) 
		MessageBox(NULL, "3", "All methods are ready!", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	if (RegGetValue(HKEY_LOCAL_MACHINE, szPath, _T(szString), RRF_RT_REG_SZ, NULL, (BYTE*)szBuf, &dwBufLen) != ERROR_SUCCESS)
		MessageBox(NULL, "3", "All methods are ready!", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	LPCSTR szStringSTD = szBuf;
#ifdef LAUNCHER_DEBUG
	MessageBox(NULL, szStringSTD, "All methods are ready!", MB_OK | MB_ICONINFORMATION);
#endif
}


void FileSystem::openReg	(_TCHAR szString[])
{
	_tsetlocale				(LC_ALL, _T( "English" ));
	/////////////////////////////////////////
	if (RegGetValue(HKEY_LOCAL_MACHINE, szPath, _T(szString), RRF_RT_REG_SZ, NULL, (BYTE*)szBuf, &dwBufLen) != ERROR_SUCCESS)
		MessageBox(NULL, "2", "2", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	LPCSTR szStringSTD = szBuf;
#ifdef LAUNCHER_DEBUG
	MessageBox(NULL, szStringSTD, "2", MB_OK | MB_ICONINFORMATION);
#endif
}


void FileSystem::writeReg	(_TCHAR szString[],
							 _TCHAR szValue[])
{
	_tsetlocale				(LC_ALL, _T( "English" ));
	/////////////////////////////////////////
	if (RegGetValue(HKEY_LOCAL_MACHINE, szPath, _T(szString), RRF_RT_REG_SZ, NULL, (BYTE*)szBuf, &dwBufLen) != ERROR_SUCCESS)
		MessageBox(NULL, "2", "2", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	if (RegSetValueEx(hKey, _T(szString), 0, REG_SZ, (BYTE*)szValue, sizeof(szValue)) != ERROR_SUCCESS) 
		MessageBox(NULL, "2", "All methods are ready!", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	if (RegCloseKey(hKey) != ERROR_SUCCESS) 
		MessageBox(NULL, "3", "All methods are ready!", MB_OK | MB_ICONINFORMATION);

	LPCSTR szStringSTD = szBuf;
#ifdef LAUNCHER_DEBUG
	MessageBox(NULL, szBuf, "All methods are ready!", MB_OK | MB_ICONINFORMATION);
#endif
}

#endif
