#pragma once
#include "xrGame/PhysicsShellHolder.h"
#include "xrPhysics/PhysicsShell.h"
#include "API/GameObject.h"
using namespace System;

namespace XRay
{
	// CPhysicsShellHolder
	public ref class PhysicalGameObject : public GameObject
	{
	public:

		PhysicalGameObject(IntPtr InNativeObject);

		property bool IsPhysicsEnabled
		{
			bool get()
			{
				return pNativeObject->PPhysicsShell()->isEnabled();
			}

			void set(bool value)
			{
				if (value)
				{
					pNativeObject->PPhysicsShell()->Enable();
				}
				else
				{
					pNativeObject->PPhysicsShell()->Disable();
				}
			}
		}

		property float Mass
		{
			float get()
			{
				return pNativeObject->PPhysicsShell()->getMass();
			}

			void set(float value)
			{
				pNativeObject->PPhysicsShell()->setMass(value);
			}
		}

	private:

		CPhysicsShellHolder* pNativeObject;
	};
}