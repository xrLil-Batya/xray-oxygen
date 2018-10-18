#pragma once
#include "xrGame/entity_alive.h"
#include "API/PhysicalGameObject.h"

using namespace System;

namespace XRay
{
	public ref class EntityAlive : public PhysicalGameObject
	{
	public:
		EntityAlive(IntPtr InNativeObject);

	private:

		CEntityAlive* pNativeObject;
	};
}