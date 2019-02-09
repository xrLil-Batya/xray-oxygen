#pragma once
#include "../xrPhysics/iphysics_scripted.h"

namespace XRay
{
	template<class T>
	ref class PhysicsGameScripted
	{
	internal:

		cphysics_game_scripted* pObj; 

	public:

		PhysicsGameScripted();
		~PhysicsGameScripted();
	};


}
