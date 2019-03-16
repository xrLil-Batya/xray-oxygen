#include "stdafx.h"
#include "PhysicsWorldScripted.h"
#include "../xrManagedGameLib/API/Level.h"

XRay::PhysicsWorldScripted::~PhysicsWorldScripted()
{

}
void XRay::PhysicsWorldScripted::AddCall(CPHCondition*c, CPHAction*a)
{
	//Level().ph_commander_physics_worldstep().add_call_threadsafety(c, a);
}

//XRay::PhysicsWorldScripted::PhysicsWorldScripted(IPHWorld* imp)
//{
//	pNativeLevel = &(cphysics_world_scripted(imp));
//}
