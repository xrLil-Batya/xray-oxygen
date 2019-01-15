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
	// А теперь давай рефакторь всё в Pascal Case стайл, никаких set и get. Все set и get - property35
	public ref class Level abstract
	{
	public:
			
		static void StartWeatherFXfromTime(::System::String^ str, float time);		
		static bool iSWfxPlaying();
		static void StopWeatherFX();
		
		static u32 	VertexInDirection(u32 level_vertex_id, Fvector direction, float max_distance);
		
		
		static float HighCoverInDirection(u32 level_vertex_id, const Fvector &direction);
		static float LowCoverInDirection(u32 level_vertex_id, const Fvector &direction);

	

		static Fvector VertexPosition(u32 level_vertex_id);

		bool ValidVertex(u32 level_vertex_id);
		void MapAddObjectSpot(u16 id, LPCSTR spot_type, LPCSTR text);
		void MapAddObjectSpotSer(u16 id, LPCSTR spot_type, LPCSTR text);
		void MapChangeSpotHint(u16 id, LPCSTR spot_type, LPCSTR text);
		void MapRemoveObjectSpot(u16 id, LPCSTR spot_type);
		u16 MapHasObjectSpot(u16 id, LPCSTR spot_type);
		bool PatrolPathExists(LPCSTR patrol_path);
		void PrefetchSnd(LPCSTR name);
		


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