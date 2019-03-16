#include "stdafx.h"
#include "EntityAlive.h"

XRay::EntityAlive::EntityAlive(IntPtr InNativeObject)
	: PhysicalGameObject(InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CEntityAlive, InNativeObject);
}
