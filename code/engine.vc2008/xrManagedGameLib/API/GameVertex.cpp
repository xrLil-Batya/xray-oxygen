#include "stdafx.h"
#include "GameVertex.h"

XRay::GameVertex::GameVertex(IntPtr InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(GameGraph::CVertex, InNativeObject);
}
