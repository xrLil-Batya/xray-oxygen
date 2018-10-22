#pragma once
#include "../xrGame/Level.h"
#include "../xrGame/level_graph.h"

using namespace System;

namespace XRay
{
	ref class LevelGraph
	{
	internal:
		CLevelGraph *pNativeGraph;

	public:
		LevelGraph(IntPtr pNativeObj) : pNativeGraph((CLevelGraph*)pNativeObj.ToPointer()) {}
		LevelGraph();

		/// <summaru> Returns Level ID</summaru>
		property u32 LevelID
		{
			u32 get()
			{
				return pNativeGraph->level_id();
			}
		}
		/// <summaru> Returns Vertex count</summaru>
		property u32 VertexCount
		{
			u32 get()
			{
				return pNativeGraph->header().vertex_count();
			}
		}
	};

	public ref class Level
	{
	internal:
		CLevel* pNativeObject;

	public:
		Level();
		Level(IntPtr InNativeObject);

	};
}