#pragma once
#include "Platform/xrPlatformUtils.h"

class xrWindowsPlatformUtils : public xrGenericPlatformUtils
{

public:
	virtual LPCSTR GetName() override;

	virtual int ShowCursor(bool bShowCursor) override;

};

extern XRCORE_API xrWindowsPlatformUtils PlatformUtils;