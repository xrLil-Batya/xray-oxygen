#include "stdafx.h"
#include "ISpectreEngineLib.h"
#include "../xrEngine/Engine.h"
#include "../xrEngine/xr_input.h"
#include "../xrEngine/x_ray.h"
#include "../xrCDB/ISpatial.h"
#include "ObjectPool.h"

FuncNode* pRootNode = nullptr;
FuncNode* pCurrentNode = nullptr;

MANAGED_ENGINE_API void xrEngineInit()
{
	InitSettings();
	InitInput(false);
	InitConsole();

	pApp = xr_new<CApplication>();
	g_SpatialSpace = xr_new<ISpatial_DB>();
	g_SpatialSpacePhysic = xr_new<ISpatial_DB>();
	InitSound1();
	InitSound2();
}


struct SpectreEngineLib : public ISpectreEngineLib
{
	virtual DWORD CreateProxyObject(DLL_Pure* pObject) override
	{
		return XRay::ObjectPool::CreateObject(IntPtr(pObject));
	}

	virtual void DestroyProxyObject(DLL_Pure* pObject) override
	{
		XRay::ObjectPool::DestroyObject(IntPtr(pObject));
	}

	virtual FuncNode* GetFunctionLinkedListStart() override
	{
		return pRootNode;
	}

};

SpectreEngineLib EngineLib;

MANAGED_ENGINE_API ISpectreEngineLib* GetEngineInterface()
{
	return &EngineLib;
}
