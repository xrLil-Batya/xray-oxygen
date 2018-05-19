/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 28 April, 2018
* xrFS.h - filesystem for launcher
* FileSystem
*************************************************/
#pragma once
/////////////////////////////////////////
#include <QSettings>
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
/////////////////////////////////////////

class FileSystem : public std::ios_base
{
private:
	std::string szReg;
	std::string szDef;

public:
	static void createGlobalQtSetting			(LPCSTR szCompany, LPCSTR szName);
	static void createAllLocalQtSettings		();
	static int  getValueSetting					(LPCSTR szPath);
	static void setValueSetting					(LPCSTR szPath, int value);
	static void createReg						(char szString[], char szValue[]);
	static void openReg							(char szString[]);
	static void writeReg						(char szString[], char szValue[]);
};
