#include "stdafx.h"
#include "API/ScriptGameObject.h"


ScriptGameObject::ScriptGameObject(::System::IntPtr isNativeLevel)
{
	CAST_TO_NATIVE_OBJECT(CScriptGameObject, isNativeLevel);
}
