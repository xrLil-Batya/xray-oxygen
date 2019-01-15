#pragma once
class CClientSpawnManager;

namespace XRay
{
	public ref class ClientSpawnManager
	{
	internal: 
		CClientSpawnManager* pNativeObject;

	public:
		ClientSpawnManager();
		ClientSpawnManager(CClientSpawnManager* pObj) : pNativeObject(pObj) {}
	};
}

