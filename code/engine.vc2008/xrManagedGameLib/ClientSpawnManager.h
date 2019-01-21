#pragma once
class CClientSpawnManager;

namespace XRay
{
	public ref class ClientSpawnManager
	{
	internal: 
		CClientSpawnManager* pNativeObject;
	public:
		delegate void Functor();

	public:
		ClientSpawnManager();
		ClientSpawnManager(CClientSpawnManager* pObj) : pNativeObject(pObj) {}

		void Add(::System::UInt16 RequestingId, ::System::UInt16 RequestedId, Functor^ Funct);
		void Del(::System::UInt16 RequestingId, ::System::UInt16 RequestedId);
		void Clear();
	};
}

