// Giperion June 2019
// [EUREKA] 3.10.2
// X-Ray Oxygen, Oxygen Team

//////////////////////////////////////////////////////////////
// Desc		: Abstract miscellaneous functions for platform
// Author	: Giperion
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////
#pragma once

class xrGenericPlatformUtils
{
public:
	/// Get platform name
	virtual LPCSTR GetName() = 0;

	/// Show or hide system mouse cursor
	virtual int ShowCursor(bool bShowCursor) = 0;

	/// Get username from subsystem
	virtual void GetUsername(string64& OutUsername) = 0;

	/// Get computer name
	virtual void GetComputerName(string64& OutComputer) = 0;

	/// Get Processor frequency in Hertz
	virtual u64  GetProcessorFrequency() = 0;

	/// Set Thread name
	virtual void SetCurrentThreadName(const string128& ThreadName) = 0;
};

