#pragma once
#include "../xrPlatformUtils.h"

class XRCORE_API xrWindowsPlatformUtils : public xrGenericPlatformUtils
{
public:
	xrWindowsPlatformUtils& operator=(const xrWindowsPlatformUtils& Other) = delete;

	virtual LPCSTR GetName() override;
	virtual int ShowCursor(bool bShowCursor) override;
	virtual void GetUsername(string64& OutUsername) override;
	virtual void GetComputerName(string64& OutComputer) override;
	virtual u64 GetProcessorFrequency() override;
	virtual void SetCurrentThreadName(const string128& ThreadName) override;

};

extern XRCORE_API xrWindowsPlatformUtils PlatformUtils;