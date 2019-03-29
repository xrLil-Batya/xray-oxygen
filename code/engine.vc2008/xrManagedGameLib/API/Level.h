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
		static ::System::UInt32 GetVertexId(Fvector position);

	};

	public ref class Level abstract
	{
	internal:
		CLevel* pNativeLevel;
	public:
		using CallBack = void(::System::String^);

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

		
		static XRay::MEnvironment^ pEnvironment();

		static XRay::EnvDescriptor^  CurrentEnvironment(XRay::MEnvironment^ self);

		static void DisableInput();
		static void EnableInput();
		static void SpawnPhantom(const Fvector &position);
		static Fbox GetBoundingVolume();
		static void IterateSounds(LPCSTR prefix, u32 max_count, CallBack callback);
		//static void IterateSounds1(LPCSTR prefix, u32 max_count, luabind::functor<void> functor);		
		//static void IterateSounds2(LPCSTR prefix, u32 max_count, luabind::object object, luabind::functor<void> functor);
		static float AddCamEffector(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func);
		static float AddCamEffector2(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func, float cam_fov);
		static void RemoveCamEffector(int id);
		static float GetSndVolume();
		static void SetSndVolume(float v);
		static void AddComplexEffector(LPCSTR section, int id);
		static void RemoveComplexEffector(int id);
		static void AddPPEffector(LPCSTR fn, int id, bool cyclic);	
		static void RemovePPEffector(int id);
		static void SetPPEffectorFactor(int id, float f, float f_sp);		
		static void SetPPEffectorFactor2(int id, float f);
		static int GCommunityGoodwill(LPCSTR _community, int _entity_id);
		static void GSetCommunityGoodwill(LPCSTR _community, int _entity_id, int val);
		static void GChangeCommunityGoodwill(LPCSTR _community, int _entity_id, int val);
		static int GGetCommunityRelation(LPCSTR comm_from, LPCSTR comm_to);
		static void GSetCommunityRelation(LPCSTR comm_from, LPCSTR comm_to, int value);
		static int GGetGeneralGoodwillBetween(u16 from, u16 to);
		static void StartTutorial(LPCSTR name);
		static void StopTutorial();
		static ::System::String^ TranslateString(LPCSTR str);
		static bool HasActiveTutotial();
		static void GSend(NET_Packet& P);
		static void UEventGen(NET_Packet& P, u32 _event, u32 _dest);
		static void UEventSend(NET_Packet& P);

	};
}