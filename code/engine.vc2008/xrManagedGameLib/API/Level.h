#pragma once
#include "ClientSpawnManager.h"
#include "Game.h"
#include "../xrManagedUILib/API/UIDialogWnd.h"


class CLevel;
class CLevelGraph;

using namespace System;

namespace XRay
{
	ref class LevelGraph abstract
	{
	public:
		/// <summaru> Returns Level ID</summaru>
		property UInt32 LevelID
		{
			UInt32 get();
		}

		/// <summaru> Returns Vertex count</summaru>
		property UInt32 VertexCount
		{
			UInt32 get();
		}
	};

	public ref class Level abstract
	{
	internal:
		CLevel* pNativeLevel;

	public:

		static property ClientSpawnManager^ ClientSpawnMngr
		{
			ClientSpawnManager^ get();
		}

		//static property UIDialogWnd^ UIDialgWnd
		//{
		//	UIDialogWnd^ get();
		//}

		/// <summary>Get Name</summary>
		property String^ Name
		{
			String^ get();
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
		static property String^ LevelName
		{
			String^ get();
		}
		/// <summary>Returns or set weather name</summary>
		static property String^ Weather
		{
			String^ get();
			void set(String^ str);
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
			ESingleGameDifficulty get();
			void set(ESingleGameDifficulty dif);
		}


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
		//static void HideIndicators();
		//static void HideIndicatorsSafe();
		//static void ShowIndicators();
		//static void ShowWeapon();
		//static void isLevelPresent();
		//static void AddCall(const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		//static void AddCall(const luabind::object &lua_object, LPCSTR condition, LPCSTR action);
		//static void AddCall(const luabind::object &lua_object, const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		//static void RevomeCall(const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		//static void RevomeCall(const luabind::object &lua_object, LPCSTR condition, LPCSTR action);
		//static void RevomeCall(const luabind::object &lua_object, const luabind::functor<bool> &condition, const luabind::functor<void> &action);
	};
}