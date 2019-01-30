#pragma once
class CLevel;
class CObjectSpace;
using namespace System;

#include "API/Object.h"

namespace XRay
{
	public ref class ObjectSpace
	{
	public:
		ObjectSpace();
		ObjectSpace(IntPtr InNativeObject);
		String^ GetHittedObjectName();
	//	XRay::Object^ GetHittedObject(); @ Indeed, but I can't implement that method until . . . 
		bool RayPick();
	private:
		CObject* current_object;
		CObjectSpace* pNativeObject;
	};


}
