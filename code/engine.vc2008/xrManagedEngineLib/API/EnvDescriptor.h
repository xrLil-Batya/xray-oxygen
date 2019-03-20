#pragma once
#include "../xrEngine/EnvDescriptor.h"


namespace XRay
{
	public ref class EnvDescriptor
	{
	internal:

		CEnvDescriptor *pNativeObject;

	public:
		EnvDescriptor(::System::IntPtr InNativeObject);

		//CurrentEnvironment(MEnvironment self);
	};
}
