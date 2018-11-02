#include "stdafx.h"
#include "Level.h"
#include "../xrGame/level_graph.h"
#include "../xrGame/Level.h"
#include "../xrGame/ai_space.h"

u32 XRay::LevelGraph::LevelID::get()
{
	return ai().level_graph().level_id();
}

u32 XRay::LevelGraph::VertexCount::get()
{
	return  ai().level_graph().header().vertex_count();
}

System::String^ XRay::Level::LevelName::get()
{
	return gcnew ::System::String(::Level().name_translated().c_str());
}