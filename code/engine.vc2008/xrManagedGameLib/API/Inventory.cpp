#include "stdafx.h"
#include "API/Inventory.h"

XRay::Inventory::Inventory(IntPtr InNativeObject)
	: NativeObject(InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CInventoryOwner, InNativeObject);
}
