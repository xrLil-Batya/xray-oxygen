#include "stdafx.h"
#include "PhysicsWorldScripted.h"
#include "../xrManagedGameLib/API/Level.h"

template<class T>
XRay::PhysicsWorldScripted::PhysicsWorldScripted()
{

}
template<class T>
XRay::PhysicsWorldScripted::~PhysicsWorldScripted()
{

}
template<class T>
void XRay::PhysicsWorldScripted::AddCall(CPHCondition*c, CPHAction*a)
{
	::Level().ph_commander_physics_worldstep().add_call_threadsafety(c, a);
}
