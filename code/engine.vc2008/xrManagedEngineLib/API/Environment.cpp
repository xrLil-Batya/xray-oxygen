#include "stdafx.h"
#include "Environment.h"
#include "../xrEngine/Environment.h"
#include "../xrEngine/editor_environment_manager.hpp"

void XRay::MEnvironment::ChangeGameTime(float fValue)
{
	::Environment().SetGameTime(::Environment().NormalizeTime((::Environment().GetGameTime() + fValue)), ::Environment().fTimeFactor);
}
/*
XRay::MEnvironment% XRay::MEnvironment::Environment()
{
	if (_pEnvironment == nullptr)
	{
		if (RDEVICE.editor())
		{
			_pEnvironment = (xr_new<editor::environment::manager>());
		}
		else
		{
			_pEnvironment = xr_new<CEnvironment>();
		}
	}

	return (XRay::MEnvironment)(_pEnvironment);
}
*/