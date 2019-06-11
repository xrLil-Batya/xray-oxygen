#include "stdafx.h"
#include "Environment.h"
#include "../xrEngine/Environment.h"

XRay::MEnvironment::MEnvironment(::System::IntPtr InNativeLevel)
{
	CAST_TO_NATIVE_OBJECT(CEnvironment, InNativeLevel);
}


void XRay::MEnvironment::ChangeGameTime(float fValue)
{
	::Environment().SetGameTime(::Environment().NormalizeTime((::Environment().GetGameTime() + fValue)), ::Environment().fTimeFactor);
}


