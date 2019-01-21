#include "stdafx.h"
#include "Spectre.h"

ISpectreCoreServer* SpectreEngineClient::CoreAPI;
ISpectreEngineLib* SpectreEngineClient::EngineLibAPI;

void SpectreEngineClient::Initialize()
{
	// Get interface ptr from xrManagedLib
	HMODULE hManagedLib = NULL;
	HMODULE hGameManagedLib = NULL;
	FARPROC pGetInterface = nullptr;
	INT_PTR pAPI = NULL;
	FuncNode* pServerNode = nullptr;

	// Get main manage core interface
	if ((hManagedLib = GetModuleHandleA("xrManagedCoreLib.dll")) == NULL)
	{
		// If managed library module is not exist - load it from bit path
		hManagedLib = LoadLibraryA("xrManagedCoreLib.dll");
		R_ASSERT2(hManagedLib, "No 'xrManagedCoreLib.dll' library at bit path.");
	}

 	pGetInterface = GetProcAddress(hManagedLib, "GetCoreInterface");
	R_ASSERT2(pGetInterface, "Can't get 'GetCoreInterface' function from xrManagedLib.dll. DLL corrupted?");
	pAPI = pGetInterface();
	CoreAPI = reinterpret_cast<ISpectreCoreServer*>(pAPI);

	// Initialize Game lib and xrScripts
	CoreAPI->LoadGameLib();
	CoreAPI->CompileScripts();

	// Get interface ptr from game lib
	if ((hGameManagedLib = GetModuleHandleA("xrManagedEngineLib.dll")) == NULL)
	{
		hGameManagedLib = LoadLibraryA("xrManagedEngineLib.dll");
		R_ASSERT(hGameManagedLib);
	}

	pGetInterface = GetProcAddress(hGameManagedLib, "GetEngineInterface");
	R_ASSERT2(pGetInterface, "Can't get 'GetGameInterface' function from xrManagedLib.dll. DLL corrupted?");
	pAPI = pGetInterface();
	EngineLibAPI = reinterpret_cast<ISpectreEngineLib*>(pAPI);

	// Get all callbacks prototype, and hook up all our interface prototypes
	pServerNode = EngineLibAPI->GetFunctionLinkedListStart();
	R_ASSERT2(pServerNode, "No Spectre callbacks in xrManaged libs");

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
}

void SpectreEngineClient::Shutdown()
{
	EngineLibAPI->OnShutdown();
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
