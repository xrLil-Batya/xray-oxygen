#pragma once
#include "../xrEngine/Environment.h"

namespace XRay
{
	public ref class MEnvironment
	{
	internal:
		static CEnvironment* pNativeLevel;

	public:

		static void ChangeGameTime(float fValue);

		MEnvironment();
		~MEnvironment();

	};

}


