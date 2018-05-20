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
char szPath[] = "Software\\xrDevLaunch\\";
char szBuf[MAX_PATH];
DWORD dwBufLen = MAX_PATH;
HKEY hKey;
static HANDLE hMutex;
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


void FileSystem::createReg	(char szString[],
							 char szValue[])
{
	/////////////////////////////////////////
	//#VERTVER: for validating regedit 
	//record, we need to use ERROR_SUCCESS
	//key. That's not fastest method, but
	//more safety.
	/////////////////////////////////////////
	setlocale				(LC_ALL,  "English" );
	hMutex					= CreateMutexA(NULL, 0, "cReg");
	if (!hMutex)
		MessageBoxA(NULL, "ERROR (FileSystem::createReg)", "ERROR", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, szPath, 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
		MessageBoxA(NULL, "1","All methods are ready!", MB_OK | MB_ICONINFORMATION);
	if (RegSetValueExA(hKey, szString, 0, REG_SZ, (BYTE*)szValue, sizeof(szValue)) != ERROR_SUCCESS) 
		MessageBoxA(NULL, "2", "All methods are ready!", MB_OK | MB_ICONINFORMATION);
	if (RegCloseKey(hKey) != ERROR_SUCCESS) 
		MessageBoxA(NULL, "3", "All methods are ready!", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	if (RegGetValueA(HKEY_LOCAL_MACHINE, szPath, szString, RRF_RT_REG_SZ, NULL, (BYTE*)szBuf, &dwBufLen) != ERROR_SUCCESS)
		MessageBoxA(NULL, "3", "All methods are ready!", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	CloseHandle(hMutex);
	//LPCSTR szStringSTD = szBuf;
#ifdef LAUNCHER_DEBUG
	MessageBox(NULL, szStringSTD, "All methods are ready!", MB_OK | MB_ICONINFORMATION);
#endif
}


void FileSystem::openReg	(char szString[])
{
	setlocale				(LC_ALL,  "English" );
	hMutex					= CreateMutexA(NULL, 0, "oReg");
	if (!hMutex)
		MessageBoxA(NULL, "ERROR (FileSystem::openReg)", "ERROR", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	if (RegGetValueA(HKEY_LOCAL_MACHINE, szPath, szString, RRF_RT_REG_SZ, NULL, (BYTE*)szBuf, &dwBufLen) != ERROR_SUCCESS)
		MessageBoxA(NULL, "2", "2", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	CloseHandle(hMutex);
	//LPCSTR szStringSTD = szBuf;
#ifdef LAUNCHER_DEBUG
	MessageBox(NULL, szStringSTD, "2", MB_OK | MB_ICONINFORMATION);
#endif
}


void FileSystem::writeReg	(char szString[],
							 char szValue[])
{
	setlocale				(LC_ALL,  "English" );
	hMutex					= CreateMutexA(NULL, 0, "wReg");
	if (!hMutex)
		MessageBoxA(NULL, "ERROR (FileSystem::writeReg)", "ERROR", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	if (RegGetValueA(HKEY_LOCAL_MACHINE, szPath, szString, RRF_RT_REG_SZ, NULL, (BYTE*)szBuf, &dwBufLen) != ERROR_SUCCESS)
		MessageBoxA(NULL, "2", "2", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	if (RegSetValueExA(hKey, szString, 0, REG_SZ, (BYTE*)szValue, sizeof(szValue)) != ERROR_SUCCESS) 
		MessageBoxA(NULL, "2", "All methods are ready!", MB_OK | MB_ICONINFORMATION);
	/////////////////////////////////////////
	if (RegCloseKey(hKey) != ERROR_SUCCESS) 
		MessageBoxA(NULL, "3", "All methods are ready!", MB_OK | MB_ICONINFORMATION);
	CloseHandle(hMutex);
	//LPCSTR szStringSTD = szBuf;
#ifdef LAUNCHER_DEBUG
	MessageBox(NULL, szBuf, "All methods are ready!", MB_OK | MB_ICONINFORMATION);
#endif
}

#endif
