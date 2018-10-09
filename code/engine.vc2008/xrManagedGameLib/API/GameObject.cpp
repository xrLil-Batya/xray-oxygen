#include "stdafx.h"
#include "GameObject.h"

XRay::GameObject::GameObject(IntPtr InNativeObject)
	: Object(InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CGameObject, InNativeObject);
}
