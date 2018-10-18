#include "stdafx.h"
#include "Actor.h"

XRay::Actor::Actor(IntPtr InNativeObject)
	:EntityAlive(InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CActor, InNativeObject);

	inventory = (Inventory^)Inventory::Create(InNativeObject, Inventory::typeid);
}

XRay::Actor::Actor() : Actor(XRay::ClassRegistrator::GetFactoryTarget())
{

}
