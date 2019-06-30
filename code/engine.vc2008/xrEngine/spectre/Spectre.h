#pragma once
#include "../../xrManagedLib/ISpectreEngine.h"
#include "../../xrManagedEngineLib/ISpectreEngineLib.h"
#include "ExternalCallbackTypes.h"

class ENGINE_API SpectreEngineClient
{
public:
	static void Initialize();
	static void Shutdown();
	static DWORD CreateProxyObject(DLL_Pure* pObject);
	static void DestroyProxyObject(DLL_Pure* pObject);

private:

	static SpectreCallback::IClientCallbackDeclaration* GetCallbackDeclForName(char* FuncName);

	static ISpectreCoreServer* CoreAPI;
	static ISpectreEngineLib* EngineLibAPI;

	static HMODULE hManagedLib;
	static HMODULE hGameManagedLib;
};

extern bool gSpectreIsLoaded;