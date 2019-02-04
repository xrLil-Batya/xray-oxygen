#pragma once
#include "ClientSpawnManager.h"
#include "Game.h"
#include "UIDialogWnd.h"
class CLevel;
class CLevelGraph;


namespace XRay
{
	ref class LevelGraph abstract
	{
	public:
		/// <summaru> Returns Level ID</summaru>
		static property ::System::UInt32 LevelID
		{
			::System::UInt32 get();
		}
		/// <summaru> Returns Vertex count</summaru>
		static property ::System::UInt32 VertexCount
		{
			::System::UInt32 get();
		}
	};

	

	public ref class Level abstract
	{
	public:

		

		static ref struct GameTime
		{
		private:

			
			//u32 was there
			::System::UInt32 _days : 9;
			::System::UInt32 _hours : 5;
			::System::UInt32 _minutes : 6;
			//::System::UInt32 _seconds : 6;

			// 9+5+6+6
			// 25bits in sum (with seconds and 20 without). it will be extend to 32bits (24) or 4 bytes (3). You can remap bites to get 32 bits if you need;
			// Remap required if this field are involved in calculation; I dont now it exactly

		public:
			static property ::System::UInt32 Days
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static property ::System::UInt32 Hours
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static property ::System::UInt32 Minutes
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static property ::System::UInt32 Seconds
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static void ChangeGameTime(u32 days, u32 hours, u32 mins);

		};

		static void StartWeatherFXfromTime(::System::String^ str, float time);		
		static bool iSWfxPlaying();
		static void StopWeatherFX();
		
		
		static float HighCoverInDirection(u32 level_vertex_id, const Fvector &direction);
		static float LowCoverInDirection(u32 level_vertex_id, const Fvector &direction);

		static ::System::Numerics::Vector3^ VertexPosition(u32 level_vertex_id);

		/// <summary>Check: Current level vertex be at level</summary>
		static bool ValidVertex(u32 level_vertex_id);

		static ::System::UInt32 	VertexInDirection(u32 level_vertex_id, Fvector direction, float max_distance);

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

		static void AddDialogToRender(UIDialogWnd^ pDialog);
		static void RemoveDialogFromRender(UIDialogWnd^ pDialog);
		static void HideIndicators();
		static void HideIndicatorsSafe();
		static void ShowIndicators();
		static void ShowWeapon();
		static void isLevelPresent();
		static void AddCall(const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		static void AddCall(const luabind::object &lua_object, LPCSTR condition, LPCSTR action);
		static void AddCall(const luabind::object &lua_object, const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		static void RevomeCall(const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		static void RevomeCall(const luabind::object &lua_object, LPCSTR condition, LPCSTR action);
		static void RevomeCall(const luabind::object &lua_object, const luabind::functor<bool> &condition, const luabind::functor<void> &action);



		static property ClientSpawnManager^ ClientSpawnMngr
		{
			ClientSpawnManager^ get();
		}

		static property UIDialogWnd^ CUIDialgWnd
		{
			UIDialogWnd^ get();
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