#pragma once
#include "../xrGame/xr_Time.h"


namespace XRay
{
	public ref class mxrTime
	{
	internal:

		xrTime* pNativeObject;

	public:

		mxrTime(::System::IntPtr inNative);
		mxrTime(xrTime* obj) : pNativeObject(obj) {};
	};
}
