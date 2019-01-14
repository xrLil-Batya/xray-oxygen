#pragma once
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
		/// <summary>Returns level name from a text files</summary>

		static void SetWeatherFX(::System::String^ str);
	
		static void StartWeatherFXfromTime(::System::String^ str, float time);
		
		static bool is_wfx_playing();

		static float Get_wfx_time();

		static void Stop_weather_fx();

		static u32 Get_time_days();
		static u32 Get_time_hours();
		static u32 Get_time_minutes();
	
		static void Change_game_time(u32 days, u32 hours, u32 mins);

		//static float High_cover_in_direction(u32 level_vertex_id, const Fvector &direction);

		//static float Low_cover_in_direction(u32 level_vertex_id, const Fvector &direction);
	
		
		static property ::System::String^ LevelName
		{
			::System::String^ get();
		}

		static property ::System::String^ Weather
		{
			::System::String^ get();
			void set(::System::String^ str);
		}

		static property float TimeFactor
		{
			float get();
			void set(float fFactor);
		}

		static property ESingleGameDifficulty GameDifficulty
		{
			ESingleGameDifficulty get ();
			void set(ESingleGameDifficulty dif);
		}

	};
}