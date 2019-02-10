#include "stdafx.h"
#include "Environment.h"
#include "../xrEngine/Environment.h"

void XRay::MEnvironment::ChangeGameTime(float fValue)
{
	::Environment().SetGameTime(::Environment().NormalizeTime((::Environment().GetGameTime() + fValue)), ::Environment().fTimeFactor);
}

