// Giperion September 2019
// [EUREKA] 3.10.2

#include "stdafx.h"
#include "xrDebugSymbol.h"
#include <DbgHelp.h>
#include <Psapi.h>
#include <stdlib.h>

xrDebugSymbols DebugSymbols;

xrDebugSymbols::xrDebugSymbols()
	: bInitialized(false)
{}

xrDebugSymbols::~xrDebugSymbols()
{
	Deinitialize();
}

void xrDebugSymbols::Deinitialize()
{
	if (bInitialized)
	{
		SymCleanup(GetCurrentProcess());
	}
}

void xrDebugSymbols::Initialize(bool bPDBShouldMatch /*= false*/)
{
	if (bInitialized)
	{
		Msg("! Trying intialize xrDebugSymbols again");
		return;
	}
	u32 ExistedOptions = SymGetOptions();

	ExistedOptions |= SYMOPT_DEFERRED_LOADS;
	ExistedOptions |= SYMOPT_AUTO_PUBLICS;

	if (bPDBShouldMatch)
	{
		ExistedOptions |= SYMOPT_EXACT_SYMBOLS;
	}
	else
	{
		ExistedOptions &= ~SYMOPT_EXACT_SYMBOLS;
	}

	ExistedOptions |= SYMOPT_LOAD_LINES;
	ExistedOptions |= SYMOPT_UNDNAME;

	SymSetOptions(ExistedOptions);

	xr_string CurrentModulePath;
	string_path strCurrentModulePath = { 0 };
	if (GetModuleFileNameA(GetModuleHandle(NULL), strCurrentModulePath, sizeof(string_path)))
	{
		CurrentModulePath.append(strCurrentModulePath);
	}

	if (!SymInitialize(GetCurrentProcess(), CurrentModulePath.c_str(), TRUE))
	{
		LPCSTR ErrorStr = Debug.error2string(GetLastError());
		Msg("Can't initialize symbol storage: %s", ErrorStr);
		return;
	}

	// load symbols for already loaded modules
	HMODULE hMods[1024];
	DWORD TotalRetBytes = 0;
	if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &TotalRetBytes))
	{
		for (int i = 0; i < (TotalRetBytes / sizeof(HMODULE)); ++i)
		{
			string_path ModulePath;
			if (GetModuleFileNameExA(GetCurrentProcess(), hMods[i], ModulePath, sizeof(ModulePath)))
			{
				LoadSymbol(ModulePath, hMods[i]);
			}
		}
	}

	bInitialized = true;
}

void xrDebugSymbols::InitializeAfterFS()
{
	// we can use command line
	if (strstr(Core.Params, "-nopdbexactmatch") != nullptr)
	{
		Deinitialize();
		Initialize(false);
	}
}

bool xrDebugSymbols::IsInitialized() const
{
	return bInitialized;
}


u16 xrDebugSymbols::GetCurrentStack(void** ppStacks, u16 StacksSize)
{
	// check if we required to capture another thread stack
	WORD frames = RtlCaptureStackBackTrace(0, StacksSize, ppStacks, NULL);

	return u16(frames);
}

u16 xrDebugSymbols::GetCallStack(HANDLE hThread, void** ppStacks, u16 StackSize)
{
	R_ASSERT(StackSize > 0x10);

	u16 StacksReceived = 0;
	auto RecordStackLambda = [&StacksReceived, ppStacks, StackSize](void* Frame) -> bool
	{
		if (StacksReceived + 1 < StackSize)
		{
			ppStacks[StacksReceived++] = Frame;
			return true;
		}
		return false;
	};

	if (GetCurrentThread() == hThread)
	{
		return GetCurrentStack(ppStacks, StackSize);
	}

	STACKFRAME64 Frame;
	ZeroMemory(&Frame, sizeof(Frame));
	CONTEXT ThreadContext;
	if (GetThreadContext(hThread, &ThreadContext))
	{
		RecordStackLambda((void*)ThreadContext.Rip); // potential dangerous. I hope 128-bit will not come soon
		Frame.AddrPC.Mode = AddrModeFlat;
		Frame.AddrPC.Offset = ThreadContext.Rip;
		Frame.AddrFrame.Mode = AddrModeFlat;
		Frame.AddrFrame.Offset = ThreadContext.Rbp;
		Frame.AddrStack.Mode = AddrModeFlat;
		Frame.AddrStack.Offset = ThreadContext.Rsp;

		while (true)
		{
			if (!StackWalk(IMAGE_FILE_MACHINE_AMD64, GetCurrentProcess(), hThread, &Frame, &ThreadContext, NULL, SymFunctionTableAccess, SymGetModuleBase64, NULL))
			{
				break;
			}

			if (!RecordStackLambda((void*)Frame.AddrPC.Offset))
			{
				break;
			}
		}
	}

	return StacksReceived;
}

void xrDebugSymbols::LoadSymbol(LPCSTR ModuleName, HMODULE hMod)
{
	if (hMod == NULL)
	{
		return;
	}

	MODULEINFO ModInfo;
	ZeroMemory(&ModInfo, sizeof(ModInfo));
	GetModuleInformation(GetCurrentProcess(), hMod, &ModInfo, sizeof(ModInfo));

	string_path ImageName = {0};
	GetModuleFileNameEx(GetCurrentProcess(), hMod, ImageName, sizeof(ImageName));

	SymLoadModuleEx(GetCurrentProcess(),
		hMod, 
		ImageName,
		ModuleName, 
		(DWORD64)ModInfo.lpBaseOfDll, 
		ModInfo.SizeOfImage, 
		nullptr, 0);
}

void xrDebugSymbols::ResolveFrame(void* Frame, string1024& OutSymbolInfo)
{
	constexpr size_t SizeOfSymbolInfo = sizeof(IMAGEHLP_SYMBOL) + sizeof(string512);
	char SymbolBuff[SizeOfSymbolInfo];
	ZeroMemory(SymbolBuff, sizeof(SymbolBuff));

	IMAGEHLP_SYMBOL* pSymbol = reinterpret_cast<IMAGEHLP_SYMBOL*>(&SymbolBuff[0]);
	pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
	pSymbol->MaxNameLength = sizeof(string512);

	IMAGEHLP_MODULE64 ModuleInfo;
	ZeroMemory(&ModuleInfo, sizeof(ModuleInfo));
	ModuleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
	if (SymGetModuleInfo64(GetCurrentProcess(), (DWORD64)Frame, &ModuleInfo))
	{
		xr_sprintf(OutSymbolInfo, "%s!", ModuleInfo.ModuleName);
	}
	else
	{
		xr_sprintf(OutSymbolInfo, "Unknown!");
	}

	if (SymGetSymFromAddr64(GetCurrentProcess(), (DWORD64)Frame, nullptr, pSymbol))
	{
		xr_strconcat(OutSymbolInfo, pSymbol->Name, "()");
	}

	IMAGEHLP_LINE LineInfo;
	ZeroMemory(&LineInfo, sizeof(LineInfo));
	LineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	LineInfo.Address = (DWORD64)Frame;

	DWORD SourceColumnNumber = 0;
	if (SymGetLineFromAddr64(GetCurrentProcess(), (DWORD64)Frame, &SourceColumnNumber, &LineInfo))
	{
		string32 LineNumStr = {0};
		_itoa_s(LineInfo.LineNumber, LineNumStr, sizeof(LineNumStr), 10);
		xr_strconcat(OutSymbolInfo, OutSymbolInfo, " - ", LineInfo.FileName, " : ", LineNumStr);
	}
	else
	{
		xr_strconcat(OutSymbolInfo, OutSymbolInfo, " - UNKHOWN : 0");
	}
}
