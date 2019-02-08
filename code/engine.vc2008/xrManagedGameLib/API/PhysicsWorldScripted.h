#pragma once
#include "../xrGame/physics_world_scripted.h"
#include "../xrManagedGameLib/PhysicsGameScripted.h"

namespace XRay
{
	template<class T>
	ref class PhysicsWorldScripted
	{

	internal:
		cphysics_world_scripted* pObj;

	public:

		PhysicsWorldScripted();
		~PhysicsWorldScripted();

		PhysicsWorldScripted(IPHWorld* imp) : (PhysicsGameScripted->pObj)<IPHWorld>(imp) {}

		float		Gravity() {return pObj->Gravity(); }
		void		SetGravity(float g) { return pObj->SetGravity(g); }
		void		AddCall(CPHCondition*c, CPHAction*a);


	};

}


