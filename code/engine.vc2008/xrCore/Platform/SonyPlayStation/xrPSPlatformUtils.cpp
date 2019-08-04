xrPSPlatformUtils//////////////////////////////////////////////////////////////
// Desc		: Sony PlayStation4 miscellaneous functions
// Author	: ForserX
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "xrPSPlatformUtils.h"

XRCORE_API xrSonyPSPlatformUtils PlatformUtils;

LPCSTR xrPSPlatformUtils::GetName()
{
	return "Sony-PS4";
}

int xrPSPlatformUtils::ShowCursor(bool bShowCursor)
{
	return ::ShowCursor(bShowCursor);
}

const char* xrPSPlatformUtils::GetRender()
{
  return "xrRender_PS4";
}
