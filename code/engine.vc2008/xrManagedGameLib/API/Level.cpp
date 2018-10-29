#include "stdafx.h"
#include "Level.h"
#include "../xrGame/ai_space.h"

XRay::Level::Level()
{
	pNativeObject = (CLevel*)&::Level();
}

XRay::Level::Level(IntPtr InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CLevel, InNativeObject);
}

XRay::LevelGraph::LevelGraph()
{
	pNativeGraph = &ai().level_graph();
}

System::String^ XRay::Level::LevelName::get()
{
	return gcnew ::System::String(pNativeObject->name_translated().c_str());
}