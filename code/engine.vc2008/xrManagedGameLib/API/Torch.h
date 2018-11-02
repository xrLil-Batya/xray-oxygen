// Rietmon: Сделано на основе ПДА. В душе не ебу, че надо переписать под фонарик, так что исправляйте сами :)
#pragma once
#include "xrGame/Torch.h"

using namespace System;;

namespace XRay
{
	public ref class Torch
	{
	internal:
	CTorch* pNativeObject;
	
	public:
		Torch(IntPtr InNativeObject);

		property bool IsEnabled
		{
			bool get()
			{
				return pNativeObject->torch_active();
			}

			void set(bool value)
			{
				torch->Switch(value);
			}
		}
	};
}