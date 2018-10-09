#include "stdafx.h"
#include "Object.h"

void XRay::Object::shedule_update(Int32 deltaTime)
{

}

XRay::Object::Object(IntPtr InNativeObject)
	: NativeObject(InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CObject, InNativeObject);
	
	AddVirtualMethod("shedule_update");
}
