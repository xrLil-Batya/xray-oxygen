#include "stdafx.h"
#include "Level.h"
#include "../xrGame/ai_space.h"

XRay::Level::Level() : Level(XRay::ClassRegistrator::GetFactoryTarget())
{
}

XRay::Level::Level(IntPtr InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CLevel, InNativeObject);
}

XRay::LevelGraph::LevelGraph()
{
	pNativeGraph = &ai().level_graph();
}
