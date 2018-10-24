#include "stdafx.h"
#include "Window.h"

XRay::Window::Window(IntPtr InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CUIWindow, InNativeObject);
}
