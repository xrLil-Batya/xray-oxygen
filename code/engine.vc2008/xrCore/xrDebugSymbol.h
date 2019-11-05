// Giperion September 2019
// [EUREKA] 3.10.2

//////////////////////////////////////////////////////////////
// Desc		: Class for working with PDB files. Useful for printing call stack
// Author	: Giperion
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////
#pragma once

class xrDebugSymbols
{
public:
	xrDebugSymbols();
	~xrDebugSymbols();

	void Deinitialize();

	void Initialize(bool bPDBShouldMatch = true);
	void InitializeAfterFS();
	bool IsInitialized() const;

	u16 GetCurrentStack(void** ppStacks, u16 StacksSize);
	u16 GetCallStack(HANDLE hThread, void** ppStacks, u16 StackSize);

	void LoadSymbol(LPCSTR ModuleName, HMODULE hMod);

	void ResolveFrame(void* Frame, string1024& OutSymbolInfo);

private:

	bool bInitialized;
};

extern xrDebugSymbols DebugSymbols;