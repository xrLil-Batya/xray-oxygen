#include "stdafx.h"
#include "../xrGame/xr_Time.h"
#include "xrTime.h"


XRay::Time::Time(::System::IntPtr inNative)
{
	CAST_TO_NATIVE_OBJECT(xrTime, inNative);
}
