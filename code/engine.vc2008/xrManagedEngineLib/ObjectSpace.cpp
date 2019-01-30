#include "stdafx.h"
#include "ObjectSpace.h"
#include "../xrGame/Level.h"

ObjectSpace::ObjectSpace()
{
	*pNativeObject = Level().ObjectSpace;
}

ObjectSpace::ObjectSpace(IntPtr InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CObjectSpace, InNativeObject);
}

