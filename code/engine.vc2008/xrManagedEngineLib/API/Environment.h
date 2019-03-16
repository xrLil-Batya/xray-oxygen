#pragma once
#include "../xrEngine/Environment.h"

namespace XRay
{
	public ref class MEnvironment
	{
	public: 

		CEnvironment* _pEnvironment = nullptr;


	public:
		static void ChangeGameTime(float fValue);
		//ENGINE_API MEnvironment% Environment();

	
	};

}



