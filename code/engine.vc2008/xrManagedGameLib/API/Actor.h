#pragma once
#include "xrServerEntities/character_info.h"
#include "xrServerEntities/specific_character.h"
#include "xrGame/Actor.h"
#include "API/EntityAlive.h"
#include "API/Inventory.h"

using namespace System;

namespace XRay
{
	public ref class Actor : public EntityAlive
	{
	internal:
		CActor* pNativeObject;

	public:
		Actor();;
		Actor(IntPtr InNativeObject);

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

		/*
		virtual void level_border_exit();
		virtual void level_border_enter();
		virtual void death();
		virtual void patrol_path_in_point();
		virtual void inventory_info();
		virtual void article_info();
		virtual void hit();
		virtual void sound();
		virtual void action_removed();
		virtual void action_movement();
		virtual void action_watch();
		virtual void action_animation();
		virtual void action_sound();
		virtual void action_particle();
		virtual void action_object();
		virtual void actor_sleep();
		virtual void helicopter_on_point();
		virtual void helicopter_on_hit();
		virtual void on_item_take();
		virtual void on_item_drop();
		virtual void script_animation();
		virtual void task_state();
		virtual void take_item_from_box();
		virtual void weapon_no_ammo();
		virtual void on_action_press();
		virtual void on_action_release();
		virtual void on_action_hold();
		virtual void map_location_added();
		*/
	public:
		Inventory^ inventory;
	};
}