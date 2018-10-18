#pragma once
#include "xrGame/GameObject.h"
#include "xrGame/ai_object_location.h"
#include "API/GameVertex.h"
//#include "xrManagedEngineLib/API/NativeObject.h"

using namespace System;

namespace XRay
{
	public ref class GameObject : public Object
	{
	public:

		GameObject(IntPtr InNativeObject);

		property UInt32 AlifeStoryId
		{
			UInt32 get()
			{
				return pNativeObject->story_id();
			}
		}

		property UInt32 LevelVertexID
		{
			UInt32 get()
			{
				return pNativeObject->ai_location().level_vertex_id();
			}
		}

		property UInt32 GameVertexID
		{
			UInt32 get()
			{
				return pNativeObject->ai_location().game_vertex_id();
			}
		}

		property GameVertex gameVertex
		{
			GameVertex get()
			{
				const GameGraph::CVertex* vertex = pNativeObject->ai_location().game_vertex();
				return GameVertex(IntPtr((void*)vertex));
			}
		}

		// Spawn time in Device.dwFrame
		property UInt32 SpawnTimeFrame
		{
			UInt32 get()
			{
				return pNativeObject->spawn_time();
			}
		}

		property bool IsAIObstacle
		{
			bool get()
			{
				return pNativeObject->is_ai_obstacle();
			}
		}

		BEGIN_DEFINE_EVENT(OnTradeStart)
		END_DEFINE_EVENT(OnTradeStart)

		BEGIN_DEFINE_EVENT(OnTradeStop)
		END_DEFINE_EVENT(OnTradeStop)

		BEGIN_DEFINE_EVENT(OnTradeSellBuyItem, GameObject^ Obj, bool bBuying, u32 MoneyTransfered)
		END_DEFINE_EVENT(OnTradeSellBuyItem, Obj, bBuying, MoneyTransfered)

		BEGIN_DEFINE_EVENT(OnTradePerformTradeOperation, u32 MoneyGet, u32 MoneyPut)
		END_DEFINE_EVENT(OnTradePerformTradeOperation, MoneyGet, MoneyPut)
		
		BEGIN_DEFINE_EVENT(OnZoneEnter, GameObject^ EnteredObject)
		END_DEFINE_EVENT(OnZoneEnter, EnteredObject)

		BEGIN_DEFINE_EVENT(OnZoneExit, GameObject^ EnteredObject)
		END_DEFINE_EVENT(OnZoneExit, EnteredObject)

		BEGIN_DEFINE_EVENT(OnLevelBorderEnter)
		END_DEFINE_EVENT(OnLevelBorderEnter)

		BEGIN_DEFINE_EVENT(OnLevelBorderExit)
		END_DEFINE_EVENT(OnLevelBorderExit)

	private:

		CGameObject* pNativeObject;
	};
}