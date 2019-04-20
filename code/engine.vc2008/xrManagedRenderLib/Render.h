#pragma once
#include "Model.h"
using namespace System;

namespace XRay
{
	// based on IRender_interface
	public ref class Render abstract
	{
	public:
		static Model^ LoadModel(XRay::File^ file, String^ name);
	};
}