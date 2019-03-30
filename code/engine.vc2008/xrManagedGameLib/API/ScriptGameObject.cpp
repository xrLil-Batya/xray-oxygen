#include "stdafx.h"
#include "ScriptGameObject.h"


XRay::ScriptGameObject::ScriptGameObject(::System::IntPtr isNativeLevel)
{
	CAST_TO_NATIVE_OBJECT(CScriptGameObject, isNativeLevel);
}
