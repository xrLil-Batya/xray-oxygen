/*----------------------------------------------------------------------
"Debugging Applications" (Microsoft Press)
Copyright (c) 1997-2000 John Robbins -- All rights reserved.
------------------------------------------------------------------------
This class is a paper-thin layer around the DBGHELP.DLL symbol engine.
This class wraps only those functions that take the unique
HANDLE value. Other DBGHELP.DLL symbol engine functions are global in
scope, so I didn’t wrap them with this class.
----------------------------------------------------------------------*/

#pragma once

#include <DbgHelp.h>
#include <tchar.h>

// Include these in case the user forgets to link against them.
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "version.lib")

// The great Bugslayer idea of creating wrapper classes on structures
// that have size fields came from fellow MSJ columnist, Paul DiLascia.
// Thanks, Paul!

// I didn’t wrap IMAGEHLP_SYMBOL because that is a variable-size
// structure.

// The IMAGEHLP_MODULE wrapper class
struct CImageHlp_Module : public IMAGEHLP_MODULE
{
	CImageHlp_Module()
	{
		memset(this, 0, sizeof(IMAGEHLP_MODULE));
		SizeOfStruct = sizeof(IMAGEHLP_MODULE);
	}
};

// The IMAGEHLP_LINE wrapper class
struct CImageHlp_Line : public IMAGEHLP_LINE
{
	CImageHlp_Line()
	{
		memset(this, 0, sizeof(IMAGEHLP_LINE));
		SizeOfStruct = sizeof(IMAGEHLP_LINE);
	}
};

// The symbol engine class
class SymbolEngine
{
	/*----------------------------------------------------------------------
	Public Construction and Destruction
	----------------------------------------------------------------------*/
public:
	// To use this class, call the SymInitialize member function to
	// initialize the symbol engine and then use the other member
	// functions in place of their corresponding DBGHELP.DLL functions.
	SymbolEngine(void) {}
	virtual ~SymbolEngine(void) {}
	/*----------------------------------------------------------------------
	Public Helper Information Functions
	----------------------------------------------------------------------*/
public:
	// Returns the file version of DBGHELP.DLL being used.
	//  To convert the return values into a readable format:
	//  wsprintf(szVer                  ,
	//             _T("%d.%02d.%d.%d"),
	//             HIWORD(dwMS)       ,
	//             LOWORD(dwMS)       ,
	//             HIWORD(dwLS)       ,
	//             LOWORD(dwLS)       );
	//  szVer will contain a string like: 5.00.1878.1
	bool GetDbgHelpVersion(DWORD& dwMS, DWORD& dwLS) { return GetInMemoryFileVersion(_T("DBGHELP.DLL"), dwMS, dwLS); }
	// Returns the file version of the PDB reading DLLs
	bool GetPDBReaderVersion(DWORD& dwMS, DWORD& dwLS)
	{
		// First try MSDBI.DLL.
		if (GetInMemoryFileVersion(_T("MSDBI.DLL"), dwMS, dwLS) == true)
		{
			return true;
		}
		else if (GetInMemoryFileVersion(_T("MSPDB60.DLL"), dwMS, dwLS) == true)
		{
			return true;
		}
		// Just fall down to MSPDB50.DLL.
		return GetInMemoryFileVersion(_T("MSPDB50.DLL"), dwMS, dwLS);
	}

	// The worker function used by the previous two functions
	bool GetInMemoryFileVersion(LPCTSTR szFile, DWORD& dwMS, DWORD& dwLS)
	{
		HMODULE hInstIH = GetModuleHandle(szFile);

		// Get the full filename of the loaded version.
		TCHAR szImageHlp[MAX_PATH];
		GetModuleFileName(hInstIH, szImageHlp, MAX_PATH);

		dwMS = 0;
		dwLS = 0;

		// Get the version information size.
		DWORD dwVerInfoHandle;
		DWORD dwVerSize;

		dwVerSize = GetFileVersionInfoSize(szImageHlp, &dwVerInfoHandle);
		if (dwVerSize == 0)
		{
			return false;
		}

		// Got the version size, now get the version information.
		LPVOID lpData = (LPVOID) new TCHAR[dwVerSize];
		if (!GetFileVersionInfo(szImageHlp, dwVerInfoHandle, dwVerSize, lpData))
		{
			delete[] lpData;
			return false;
		}

		VS_FIXEDFILEINFO* lpVerInfo;
		UINT uiLen;
		bool bRet = !!VerQueryValue(lpData, _T("\\"), (LPVOID*)&lpVerInfo, &uiLen);
		if (bRet)
		{
			dwMS = lpVerInfo->dwFileVersionMS;
			dwLS = lpVerInfo->dwFileVersionLS;
		}

		delete[] lpData;
		return bRet;
	}

	/*----------------------------------------------------------------------
	Public Initialization and Cleanup
	----------------------------------------------------------------------*/
public:
	bool SymInitialize(IN HANDLE hProcess, IN char* UserSearchPath, IN bool fInvadeProcess)
	{
		m_hProcess = hProcess;
		return !!::SymInitialize(hProcess, UserSearchPath, fInvadeProcess);
	}

	bool SymCleanup() { return !!::SymCleanup(m_hProcess); }
	/*----------------------------------------------------------------------
	Public Module Manipulation
	----------------------------------------------------------------------*/
public:
	bool SymEnumerateModules(IN PSYM_ENUMMODULES_CALLBACK EnumModulesCallback, IN PVOID UserContext)
	{
		return !!::SymEnumerateModules(m_hProcess, EnumModulesCallback, UserContext);
	}

	bool SymLoadModule(IN HANDLE hFile, IN PSTR ImageName, IN PSTR ModuleName, IN DWORD BaseOfDll, IN DWORD SizeOfDll)
	{
		return !!::SymLoadModule(m_hProcess, hFile, ImageName, ModuleName, BaseOfDll, SizeOfDll);
	}

	bool EnumerateLoadedModules(IN PENUMLOADED_MODULES_CALLBACK EnumLoadedModulesCallback, IN PVOID UserContext)
	{
		return !!::EnumerateLoadedModules(m_hProcess, EnumLoadedModulesCallback, UserContext);
	}

	bool SymUnloadModule(IN DWORD BaseOfDll) { return !!::SymUnloadModule(m_hProcess, BaseOfDll); }
	bool SymGetModuleInfo(IN DWORD dwAddr, OUT PIMAGEHLP_MODULE ModuleInfo)
	{
		return !!::SymGetModuleInfo(m_hProcess, dwAddr, ModuleInfo);
	}

	DWORD SymGetModuleBase(IN DWORD dwAddr) { return ::SymGetModuleBase(m_hProcess, dwAddr); }
	/*----------------------------------------------------------------------
	Public Symbol Manipulation
	----------------------------------------------------------------------*/
public:
	bool SymEnumerateSymbols(IN DWORD BaseOfDll, IN PSYM_ENUMSYMBOLS_CALLBACK EnumSymbolsCallback, IN PVOID UserContext)
	{
		return !!::SymEnumerateSymbols(m_hProcess, BaseOfDll, EnumSymbolsCallback, UserContext);
	}

	bool SymGetSymFromAddr(IN DWORD dwAddr, OUT PDWORD_PTR pdwDisplacement, OUT PIMAGEHLP_SYMBOL Symbol)
	{
		return !!::SymGetSymFromAddr(m_hProcess, dwAddr, pdwDisplacement, Symbol);
	}

	bool SymGetSymFromName(IN char* Name, OUT PIMAGEHLP_SYMBOL Symbol)
	{
		return !!::SymGetSymFromName(m_hProcess, Name, Symbol);
	}

	bool SymGetSymNext(IN OUT PIMAGEHLP_SYMBOL Symbol) { return !!::SymGetSymNext(m_hProcess, Symbol); }
	bool SymGetSymPrev(IN OUT PIMAGEHLP_SYMBOL Symbol) { return !!::SymGetSymPrev(m_hProcess, Symbol); }
	/*----------------------------------------------------------------------
	Public Source Line Manipulation
	----------------------------------------------------------------------*/
public:
	bool SymGetLineFromAddr(IN DWORD dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE Line)
	{
#ifdef DO_NOT_WORK_AROUND_SRCLINE_BUG
		// Just pass along the values returned by the main function.
		return !!::SymGetLineFromAddr(m_hProcess, dwAddr, pdwDisplacement, Line);

#else
		// The problem is that the symbol engine finds only those source
		// line addresses (after the first lookup) that fall exactly on
		// a zero displacement. I’ll walk backward 100 bytes to
		// find the line and return the proper displacement.
		DWORD dwTempDis = 0;
		while (!::SymGetLineFromAddr(m_hProcess, dwAddr - dwTempDis, pdwDisplacement, Line))
		{
			dwTempDis += 1;
			if (100 == dwTempDis)
				return false;
		}
		// I found it and the source line information is correct, so
		// change the displacement if I had to search backward to find
		// the source line.
		if (dwTempDis != 0)
		{
			*pdwDisplacement = dwTempDis;
		}
		return true;
#endif // DO_NOT_WORK_AROUND_SRCLINE_BUG
	}

	bool SymGetLineFromName(IN char* ModuleName, IN char* FileName, IN DWORD dwLineNumber, OUT PLONG plDisplacement,
		IN OUT PIMAGEHLP_LINE Line)
	{
		return !!::SymGetLineFromName(m_hProcess, ModuleName, FileName, dwLineNumber, plDisplacement, Line);
	}

	bool SymGetLineNext(IN OUT PIMAGEHLP_LINE Line) { return !!::SymGetLineNext(m_hProcess, Line); }
	bool SymGetLinePrev(IN OUT PIMAGEHLP_LINE Line) { return !!::SymGetLinePrev(m_hProcess, Line); }
	bool SymMatchFileName(IN char* FileName, IN char* Match, OUT char** FileNameStop, OUT char** MatchStop)
	{
		return !!::SymMatchFileName(FileName, Match, FileNameStop, MatchStop);
	}

	/*----------------------------------------------------------------------
	Public Miscellaneous Members
	----------------------------------------------------------------------*/
public:
	LPVOID SymFunctionTableAccess(DWORD AddrBase) { return ::SymFunctionTableAccess(m_hProcess, AddrBase); }
	bool SymGetSearchPath(OUT char* SearchPath, IN DWORD SearchPathLength)
	{
		return !!::SymGetSearchPath(m_hProcess, SearchPath, SearchPathLength);
	}

	bool SymSetSearchPath(IN char* SearchPath) { return ::SymSetSearchPath(m_hProcess, SearchPath); }
#ifdef _M_X64
	bool SymRegisterCallback(IN PSYMBOL_REGISTERED_CALLBACK CallbackFunction, IN ULONG64 UserContext)
#else
	bool SymRegisterCallback(IN PSYMBOL_REGISTERED_CALLBACK CallbackFunction, IN PVOID UserContext)
#endif
	{
		return !!::SymRegisterCallback(m_hProcess, CallbackFunction, UserContext);
	}
	/*----------------------------------------------------------------------
	Protected Data Members
	----------------------------------------------------------------------*/
protected:
	// The unique value that will be used for this instance of the
	// symbol engine. This value doesn’t have to be an actual
	// process value, just a unique value.
	HANDLE m_hProcess;
};