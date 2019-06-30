#include "stdafx.h"
#include "Spectre.h"

ISpectreCoreServer* SpectreEngineClient::CoreAPI;
ISpectreEngineLib* SpectreEngineClient::EngineLibAPI;

HMODULE SpectreEngineClient::hManagedLib = NULL;

HMODULE SpectreEngineClient::hGameManagedLib = NULL;

bool gSpectreIsLoaded = false;

void SpectreEngineClient::Initialize()
{
	// Get interface ptr from xrManagedLib

	FARPROC pGetInterface = nullptr;
	INT_PTR pAPI = NULL;
	FuncNode* pServerNode = nullptr;

	// Get main manage core interface
	hManagedLib = GetModuleHandle("xrManagedCoreLib.dll");
	if (!hManagedLib)
		hManagedLib = LoadLibrary("xrManagedCoreLib.dll");
	
	R_ASSERT2(hManagedLib, "No 'xrManagedCoreLib.dll' library at bit path.");

	pGetInterface = GetProcAddress(hManagedLib, "GetCoreInterface");
	R_ASSERT2(pGetInterface, "Can't get 'GetCoreInterface' function from xrManagedLib.dll. DLL corrupted?");

	if (!pGetInterface) return;

	pAPI = pGetInterface();
	CoreAPI = reinterpret_cast<ISpectreCoreServer*>(pAPI);

	// Initialize Game lib and xrScripts
	CoreAPI->LoadGameLib();
	if (Core.bSpectreEnabled)
	{
		CoreAPI->CompileScripts();
	}

	// Get interface ptr from game lib
	hGameManagedLib = GetModuleHandle("xrManagedEngineLib.dll");
	if (!hGameManagedLib)
		hGameManagedLib = LoadLibrary("xrManagedEngineLib.dll");
	
	R_ASSERT(hGameManagedLib);

	pGetInterface = GetProcAddress(hGameManagedLib, "GetEngineInterface");
	R_ASSERT2(pGetInterface, "Can't get 'GetGameInterface' function from xrManagedLib.dll. DLL corrupted?");
	if (!pGetInterface) return;

	pAPI = pGetInterface();
	EngineLibAPI = reinterpret_cast<ISpectreEngineLib*>(pAPI);

	// Initialize render lib statics
	HMODULE hRenderModule = GetModuleHandle("xrManagedRenderLib.dll");
	FARPROC hDummyFunctionFunc = GetProcAddress(hRenderModule, "DummyFunction");
	hDummyFunctionFunc();

	// Get all callbacks prototype, and hook up all our interface prototypes
	pServerNode = EngineLibAPI->GetFunctionLinkedListStart();
	R_ASSERT2(pServerNode, "No Spectre callbacks in xrManaged libs");
	if (!pServerNode) return;

	do
	{
		SpectreCallback::IClientCallbackDeclaration* pClientNode = GetCallbackDeclForName(pServerNode->Name);
		if (pClientNode != nullptr)
		{
			pClientNode->InitializeClientCallback(pServerNode->FuncInvokerInst);
		}
		else
		{
			Msg("! Managed callback \"%s\" will not work - XRay doesn't have client version of that", pServerNode->Name);
		}

		pServerNode = pServerNode->NextNode;
	} while (pServerNode != nullptr);

	gSpectreIsLoaded = true;
}

void SpectreEngineClient::Shutdown()
{
	EngineLibAPI->OnShutdown();
	CoreAPI->Shutdown();
	FreeLibrary(hGameManagedLib); hGameManagedLib = NULL;
	FreeLibrary(hManagedLib); hManagedLib = NULL;
}

DWORD SpectreEngineClient::CreateProxyObject(DLL_Pure* pObject)
{
 	return EngineLibAPI->CreateProxyObject(pObject);
}

void SpectreEngineClient::DestroyProxyObject(DLL_Pure* pObject)
{
	EngineLibAPI->DestroyProxyObject(pObject);
}

SpectreCallback::IClientCallbackDeclaration* SpectreEngineClient::GetCallbackDeclForName(char* FuncName)
{
	SpectreCallback::IClientCallbackDeclaration* pClientFuncNode = SpectreCallback::pRootNode;
	R_ASSERT2(pClientFuncNode, "No Spectre callbacks?");

	do 
	{
		if (strcmp(FuncName, pClientFuncNode->FunctionName) == 0)
		{
			return pClientFuncNode;
		}
		pClientFuncNode = pClientFuncNode->pNextNode;
	} while (pClientFuncNode != nullptr);

	return nullptr;
}
