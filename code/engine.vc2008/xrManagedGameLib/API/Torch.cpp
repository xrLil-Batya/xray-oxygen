// Rietmon: Сделано на основе ПДА. В душе не ебу, че надо переписать под фонарик, так что исправляйте сами :)
#include "stdafx.h"
#include "API/Torch.h"

XRay::Torch::Torch(IntPtr InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CTorch, InNativeObject);
}
