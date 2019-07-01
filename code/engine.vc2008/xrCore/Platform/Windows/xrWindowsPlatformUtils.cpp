#include "stdafx.h"
#include "xrWindowsPlatformUtils.h"

XRCORE_API xrWindowsPlatformUtils PlatformUtils;

LPCSTR xrWindowsPlatformUtils::GetName()
{
	return "Windows-x64";
}

void xrWindowsPlatformUtils::ShowCursor(bool bShowCursor)
{
	::ShowCursor(bShowCursor);
}
