#include "stdafx.h"
#include "PhysicalGameObject.h"

XRay::PhysicalGameObject::PhysicalGameObject(IntPtr InNativeObject)
	: XRay::GameObject(InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CPhysicsShellHolder, InNativeObject);
}
