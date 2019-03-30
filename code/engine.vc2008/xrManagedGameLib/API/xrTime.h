#pragma once

namespace XRay
{
	public ref class Time
	{
	internal:

		class xrTime* pNativeObject;

	public:

		Time(::System::IntPtr inNative);
		Time(xrTime* obj) : pNativeObject(obj) {};
	};
}
