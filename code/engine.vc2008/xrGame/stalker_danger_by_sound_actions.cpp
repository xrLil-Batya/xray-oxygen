////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_by_sound_actions.cpp
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger by sound actions classes
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "stalker_danger_by_sound_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

#include "stalker_movement_manager_smart_cover.h"
#include "sight_manager.h"
#include "object_handler.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundListenTo
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerBySoundListenTo::CStalkerActionDangerBySoundListenTo(CAI_Stalker* object, LPCSTR action_name) :
	inherited(object, action_name)
{
}

void CStalkerActionDangerBySoundListenTo::initialize()
{
	inherited::initialize();
	object().movement().set_desired_direction(0);
	object().movement().set_path_type(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state(eBodyStateStand);
	object().movement().set_movement_type(eMovementTypeStand);
	object().movement().set_mental_state(eMentalStateDanger);
	object().sight().setup(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal(eObjectActionIdle);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundCheck
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerBySoundCheck::CStalkerActionDangerBySoundCheck(CAI_Stalker* object, LPCSTR action_name) :
	inherited(object, action_name)
{
}

void CStalkerActionDangerBySoundCheck::initialize()
{
	inherited::initialize();
	object().movement().set_desired_direction(0);
	object().movement().set_path_type(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state(eBodyStateStand);
	object().movement().set_movement_type(eMovementTypeStand);
	object().movement().set_mental_state(eMentalStateDanger);
	object().sight().setup(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal(eObjectActionIdle);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundTakeCover
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerBySoundTakeCover::CStalkerActionDangerBySoundTakeCover(CAI_Stalker* object, LPCSTR action_name) :
	inherited(object, action_name)
{
}

void CStalkerActionDangerBySoundTakeCover::initialize()
{
	inherited::initialize();
	object().movement().set_desired_direction(0);
	object().movement().set_path_type(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state(eBodyStateStand);
	object().movement().set_movement_type(eMovementTypeStand);
	object().movement().set_mental_state(eMentalStateDanger);
	object().sight().setup(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal(eObjectActionIdle);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundLookOut
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerBySoundLookOut::CStalkerActionDangerBySoundLookOut(CAI_Stalker* object, LPCSTR action_name) :
	inherited(object, action_name)
{
}

void CStalkerActionDangerBySoundLookOut::initialize()
{
	inherited::initialize();
	object().movement().set_desired_direction(0);
	object().movement().set_path_type(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state(eBodyStateStand);
	object().movement().set_movement_type(eMovementTypeStand);
	object().movement().set_mental_state(eMentalStateDanger);
	object().sight().setup(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal(eObjectActionIdle);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundLookAround
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerBySoundLookAround::CStalkerActionDangerBySoundLookAround(CAI_Stalker* object, LPCSTR action_name) :
	inherited(object, action_name)
{
}

void CStalkerActionDangerBySoundLookAround::initialize()
{
	inherited::initialize();
	object().movement().set_desired_direction(0);
	object().movement().set_path_type(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state(eBodyStateStand);
	object().movement().set_movement_type(eMovementTypeStand);
	object().movement().set_mental_state(eMentalStateDanger);
	object().sight().setup(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal(eObjectActionIdle);
}