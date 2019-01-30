#include "stdafx.h"
#include "ObjectSpace.h"
#include "../xrGame/xrGame.h"
#include "../xrGame/Level.h"

namespace XRay
{
	ObjectSpace::ObjectSpace()
	{
		*pNativeObject = Level().ObjectSpace;
		current_object = nullptr;
	}

	ObjectSpace::ObjectSpace(IntPtr InNativeObject)
	{
		CAST_TO_NATIVE_OBJECT(CObjectSpace, InNativeObject);
	}

	String^ ObjectSpace::GetHittedObjectName()
	{
		if (current_object)
		{
			String^ result = gcnew String(current_object->cName().data());
			return result;
		}
		else
		{
			return nullptr;
		}

	}

	bool ObjectSpace::RayPick()
	{
		// @ just reference to R;
 
		return false;
	}

}

