#pragma once
#include "ClientSpawnManager.h"
#include "PhysicsWorldScripted.h"
#include "Game.h"

class CLevel;
class CLevelGraph;



namespace XRay
{
	
	ref class LevelGraph abstract
	{
	public:
		/// <summaru> Returns Level ID</summaru>
		property ::System::UInt32 LevelID
		{
			::System::UInt32 get();
		}

		/// <summaru> Returns Vertex count</summaru>
		property ::System::UInt32 VertexCount
		{
			::System::UInt32 get();
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
		property ::System::String^ Name
		{
			::System::String^ get();
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



		static XRay::PhysicsWorldScripted^ physicsWorldScripted();



		static void AddDialogToRender(XRay::UIDialogWnd^ pDialog);
		static void RemoveDialogFromRender(XRay::UIDialogWnd^ pDialog);

		static void HideIndicators();
		static void HideIndicatorsSafe();
		static void ShowIndicators();
		static void ShowWeapon(bool b);
		static bool isLevelPresent();
		//static void AddCall(const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		//static void AddCall(const luabind::object &lua_object, LPCSTR condition, LPCSTR action);
		//static void AddCall(const luabind::object &lua_object, const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		//static void RemoveCall(const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		//static void RemoveCall(const luabind::object &lua_object, LPCSTR condition, LPCSTR action);
		//static void RemoveCall(const luabind::object &lua_object, const luabind::functor<bool> &condition, const luabind::functor<void> &action);
		//static void RemoveCallForObject(const luabind::object &lua_object);

		
		static MEnvironment^ pEnvironment();

		//static EnvDescriptor^ CurrentEnvironment(CEnvironment* self);
		////extern bool g_bDisableAllInput;
		//static void DisableInput();
		//static void EnableInput();
		//static void SpawnPhantom(const Fvector &position);

	};
}