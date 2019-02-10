#include "stdafx.h"
#include "Environment.h"


XRay::MEnvironment::MEnvironment()
{
}


XRay::MEnvironment::~MEnvironment()
{
}

void XRay::MEnvironment::ChangeGameTime(float fValue)
{
	pNativeLevel->SetGameTime(pNativeLevel->NormalizeTime((pNativeLevel->GetGameTime() + fValue)), pNativeLevel->fTimeFactor);
}

