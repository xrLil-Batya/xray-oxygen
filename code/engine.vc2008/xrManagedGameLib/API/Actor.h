#pragma once
#include "xrServerEntities/character_info.h"
#include "xrServerEntities/specific_character.h"
#include "xrGame/Actor.h"
#include "API/EntityAlive.h"
#include "API/Inventory.h"

class CActor;
using namespace System;

namespace XRay
{
	public ref class Actor : public EntityAlive
	{
	internal:
		CActor* pNativeObject;

	public:
		Actor();
		Actor(IntPtr InNativeObject);

		static property float Satiety
		{
			float get();
			void set(float fNewValue);
		}

		static property bool bInputActive
		{
			bool get();
			void set(bool bInput);
		}

	public:
		Inventory^ inventory;

	//Callbacks
	public:
		//virtual void shedule_Update(u32 Interval);
		virtual void UseObject(Object^ pObj);

		virtual void TradeStart();
		virtual void TradeStop();
		virtual void TradeSellBuyItem();
		virtual void TradePerformOperation();

		virtual void TraderGlobalAnimRequest();
		virtual void TraderHeadAnimRequest();
		virtual void TraderSoundEend();

		virtual void ZoneEnter();
		virtual void ZoneExit();
	};
}