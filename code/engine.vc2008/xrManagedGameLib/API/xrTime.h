#pragma once
class xrTime;

namespace XRay
{
	public ref class Time
	{
	internal:
		class xrTime* pNativeObject;

	public:

		Time(::System::IntPtr inNative);
	//	Time() : pNativeObject(new xrTime()) {};
	};
}
