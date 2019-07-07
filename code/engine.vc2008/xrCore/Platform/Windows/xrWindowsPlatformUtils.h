#pragma once
#include "../xrPlatformUtils.h"

class XRCORE_API xrWindowsPlatformUtils : public xrGenericPlatformUtils
{
public:
	virtual LPCSTR GetName() override;
	virtual int ShowCursor(bool bShowCursor) override;

	xrWindowsPlatformUtils& operator=(const xrWindowsPlatformUtils& Other) = delete;
};

extern XRCORE_API xrWindowsPlatformUtils PlatformUtils;