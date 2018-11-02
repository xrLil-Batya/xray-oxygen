#pragma once
class CLevel;
class CLevelGraph;

using namespace System;

namespace XRay
{
	ref class LevelGraph abstract
	{
	public:
		/// <summaru> Returns Level ID</summaru>
		static property u32 LevelID
		{
			u32 get();
		}
		/// <summaru> Returns Vertex count</summaru>
		static property u32 VertexCount
		{
			u32 get();
		}
	};

	public ref class Level abstract
	{
	public:
		/// <summary>Returns level name from a text files</summary>
		static property ::System::String^ LevelName
		{
			::System::String^ get();
		}
	};
}