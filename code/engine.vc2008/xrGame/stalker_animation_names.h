////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_names.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation names
////////////////////////////////////////////////////////////////////////////
#pragma once

extern constexpr char* state_names			[];
extern constexpr char* weapon_names			[];
extern constexpr char* weapon_action_names	[];
extern constexpr char* food_names			[];
extern constexpr char* food_action_names	[];
extern constexpr char* movement_names		[];
extern constexpr char* movement_action_names[];
extern constexpr char* in_place_names		[];
extern constexpr char* global_names			[];
extern constexpr char* head_names			[];

enum ECriticalWoundType 
{
	critical_wound_type_head		= u32(4),
	critical_wound_type_torso,
	critical_wound_type_hand_left,
	critical_wound_type_hand_right,
	critical_wound_type_leg_left,
	critical_wound_type_leg_right,
	critical_wound_type_dummy		= u32(-1),
};
