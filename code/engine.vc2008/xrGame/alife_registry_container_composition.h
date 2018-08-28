////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_container_composition.h
//	Created 	: 01.07.2004
//  Modified 	: 01.07.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry container class, composition of the registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InfoPortionDefs.h"
#include "PdaMsg.h"

#include "character_info_defs.h"
#include "relation_registry_defs.h"

#include "gametaskdefs.h"
#include "game_news.h"
#include "map_location_defs.h"

extern "C"
{
#include "lua/lauxlib.h"
}
#include "luabind/luabind.hpp"

#pragma warning(push)
#pragma warning(disable:4005)

template <typename _index_type, typename _data_type>
class CALifeAbstractRegistry;

//для всех персонажей, те порции информации, которые они помнят
using CInfoPortionRegistry = CALifeAbstractRegistry<u16, KNOWN_INFO_VECTOR >;
add_to_registry_type_list(CInfoPortionRegistry)
#define info_portions define_constant(CInfoPortionRegistry) 
#define registry_type_list save_registry_type_list(CInfoPortionRegistry)

//для всех персонажей, отношения с другими персонажами
using CRelationRegistry = CALifeAbstractRegistry<u16, RELATION_DATA >;
add_to_registry_type_list(CRelationRegistry)
#define character_relations define_constant(CRelationRegistry) 
#define registry_type_list save_registry_type_list(CRelationRegistry)

//список новостей полученных актером, состоит из новостей симуляции и сюжетных (скриптованых) новостей 
using CGameNewsRegistry = CALifeAbstractRegistry<u16, GAME_NEWS_VECTOR >;
add_to_registry_type_list(CGameNewsRegistry)
#define game_news define_constant(CGameNewsRegistry) 
#define registry_type_list save_registry_type_list(CGameNewsRegistry)


//список описаний персонажей, которые уже задействованы в игре
using CSpecificCharacterRegistry = CALifeAbstractRegistry<shared_str, int >;
add_to_registry_type_list(CSpecificCharacterRegistry)
#define specific_characters define_constant(CSpecificCharacterRegistry) 
#define registry_type_list save_registry_type_list(CSpecificCharacterRegistry)

//map locations for actor
add_to_registry_type_list(CMapLocationRegistry)
#define map_locations define_constant(CMapLocationRegistry) 
#define registry_type_list save_registry_type_list(CMapLocationRegistry)

//game tasks for actor
add_to_registry_type_list(CGameTaskRegistry)
#define map_locations define_constant(CGameTaskRegistry) 
#define registry_type_list save_registry_type_list(CGameTaskRegistry)

#pragma warning(pop)