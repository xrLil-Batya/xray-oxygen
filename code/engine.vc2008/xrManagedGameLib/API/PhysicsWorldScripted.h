#pragma once
#include "../xrGame/physics_world_scripted.h"
#include "../xrManagedGameLib/API/PhysicsGameScripted.h"

namespace XRay
{
	
	public ref class PhysicsWorldScripted
	//	:public PhysicsGameScripted<IPHWorld>
	{

	internal:
		cphysics_world_scripted* pNativeLevel;

	public:

		~PhysicsWorldScripted();

		//PhysicsWorldScripted(IPHWorld* imp);
	
// 		PhysicsWorldScripted(IPHWorld* imp):PhysicsGameScripted<IPHWorld>(imp) {};
 		PhysicsWorldScripted(cphysics_world_scripted* imp) : pNativeLevel(imp) {};

		float		Gravity() {return pNativeLevel->Gravity(); }
		void		SetGravity(float g) { return pNativeLevel->SetGravity(g); }
		void		AddCall(CPHCondition*c, CPHAction*a);

		::System::IntPtr GetNative() {
			return ::System::IntPtr(pNativeLevel
			);
		}
	};

}


