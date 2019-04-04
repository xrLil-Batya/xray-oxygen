#pragma once
#include "ClientSpawnManager.h"
#include "PhysicsWorldScripted.h"
#include "ScriptGameObject.h"
#include "API/xrTime.h"
#include "API/WeatherFX.h"
#include "Game.h"

class CLevel;
class CLevelGraph;
class CPatrolPath;



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

		static void PatrolPathAdd(LPCSTR patrol_path, CPatrolPath* path);
		static void PatrolPathRemove(LPCSTR patrol_path);
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

		/// <summary>Get Name</summary>
		property ::System::String^ Name
		{
			::System::String^ get();
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

		static property GameObject^ ViewEntity
		{
			GameObject^ get();
			void set(GameObject^);
		}

		static float HighCoverInDirection(u32 level_vertex_id, const Fvector &direction);
		static float LowCoverInDirection(u32 level_vertex_id, const Fvector &direction);

		static ::System::Numerics::Vector3^ VertexPosition(u32 level_vertex_id);

		/// <summary>Check: Current level vertex be at level</summary>
		static bool ValidVertex(u32 level_vertex_id);
		static ::System::UInt32 VertexInDirection(u32 level_vertex_id, Fvector direction, float max_distance);
		
		static bool PatrolPathExists(LPCSTR patrol_path);
		static XRay::PhysicsWorldScripted^ physicsWorldScripted();

		static void AddDialogToRender(XRay::UIDialogWnd^ pDialog);
		static void RemoveDialogFromRender(XRay::UIDialogWnd^ pDialog);
		static void SpawnPhantom(const Fvector &position);
	};
}