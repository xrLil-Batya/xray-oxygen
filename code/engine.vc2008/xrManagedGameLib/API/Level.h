#pragma once
#include "ClientSpawnManager.h"
#include "Game.h"
class CLevel;
class CLevelGraph;

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
		static void StartWeatherFXfromTime(::System::String^ str, float time);		
		static bool iSWfxPlaying();
		static void StopWeatherFX();
		
		
		static float HighCoverInDirection(u32 level_vertex_id, const Fvector &direction);
		static float LowCoverInDirection(u32 level_vertex_id, const Fvector &direction);

		static ::System::Numerics::Vector3^ VertexPosition(u32 level_vertex_id);

		/// <summary>Check: Current level vertex be at level</summary>
		static bool ValidVertex(u32 level_vertex_id);

		static u32 	VertexInDirection(u32 level_vertex_id, Fvector direction, float max_distance);

		// Map
		/// <summary>Check: Map has is current spot by object ID?</summary>
		static bool MapHasObjectSpot(u16 id, LPCSTR spot_type);
		/// <summary>Set: Set spot to level map by object ID</summary>
		static void MapAddObjectSpot(u16 id, LPCSTR spot_type, LPCSTR text);
		/// <summary>Set: Del spot from level map by object ID</summary>
		static void MapRemoveObjectSpot(u16 id, LPCSTR spot_type);
		/// <summary>Set: Set spot to level map by object ID</summary>
		static void MapAddObjectSpotSer(u16 id, LPCSTR spot_type, LPCSTR text);
		/// <summary>Set: Change spot hint from level map by object ID</summary>
		static void MapChangeSpotHint(u16 id, LPCSTR spot_type, LPCSTR text);

		static bool PatrolPathExists(LPCSTR patrol_path);
		static void PrefetchSnd(LPCSTR name);
		
		static property ClientSpawnManager^ ClientSpawnMngr
		{
			ClientSpawnManager^ get();
		}

		/// <summary>Get Name</summary>
		static property  LPCSTR Name
		{
			LPCSTR get();
		}
		/// <summary>Set WeatherFX</summary>
		static property ::System::String^ WeatherFX
		{
			void set(::System::String^ str);
		}
		/// <summary>Return WfxTime</summary>
		static property float WfxTime
		{
			float get();
		}
		/// <summary>Returns rain factor</summary>
		static property float RainFactor
		{
			float get();
		}
		/// <summary>Returns level name from a text files</summary>
		static property ::System::String^ LevelName
		{
			::System::String^ get();
		}
		/// <summary>Returns or set weather name</summary>
		static property ::System::String^ Weather
		{
			::System::String^ get();
			void set(::System::String^ str);
		}
		/// <summary>Get or set time factor</summary>
		static property float TimeFactor
		{
			float get();
			void set(float fFactor);
		}
		/// <summary>Get or set game difficulty</summary>
		static property ESingleGameDifficulty GameDifficulty
		{
			ESingleGameDifficulty get ();
			void set(ESingleGameDifficulty dif);
		}
	};
}