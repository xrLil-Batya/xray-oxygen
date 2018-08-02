////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator.cpp
//	Created 	: 25.12.2002
//  Modified 	: 06.04.2017
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_simulator.h"
#include "xrServer_Objects_ALife.h"
#include "ai_space.h"
#include "GamePersistent.h"
#include "script_engine.h"
#include "mainmenu.h"
#include "object_factory.h"
#include "alife_object_registry.h"
#include "../xrEngine/xr_ioconsole.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_graph_registry.h"
#include "alife_schedule_registry.h"
#ifdef DEBUG
#	include "moving_objects.h"
#endif // DEBUG

const char* alife_section = "alife";
extern void destroy_lua_wpn_params();

CALifeSimulator::CALifeSimulator(xrServer *server, shared_str *command_line) : 
	CALifeUpdateManager(server, alife_section), CALifeSimulatorBase(server, alife_section)
{
    if (strstr(Core.Params, "-keep_lua"))
    {
        destroy_lua_wpn_params();
        MainMenu()->DestroyInternal(true);
        xr_delete(g_object_factory);
        ai().script_engine().init();

#ifdef DEBUG
        ai().moving_objects().clear();
#endif // DEBUG
    }

	ai().set_alife				(this);

	typedef IGame_Persistent::params params;
	params &p = g_pGamePersistent->m_game_params;
	
	R_ASSERT2(xr_strlen(p.m_game_or_spawn) && !xr_strcmp(p.m_alife,"alife") && !xr_strcmp(p.m_game_type,"single"), "Invalid server options!");
	
	string256 temp;
	xr_strcpy(temp,p.m_game_or_spawn);
	xr_strcat(temp,"/");
	xr_strcat(temp,p.m_game_type);
	xr_strcat(temp,"/");
	xr_strcat(temp,p.m_alife);
    GamePersistent().SetServerOption(temp);
	
	LPCSTR start_game_callback = pSettings->r_string(alife_section,"start_game_callback");
	luabind::functor<void> functor;
	R_ASSERT2(ai().script_engine().functor(start_game_callback,functor),"Failed to get start game callback");

// 	try
// 	{
		functor();
// 	}
// 	catch (luabind::error err)
// 	{
// 		R_ASSERT3(false, "Failed call start game callback. %s", err.what());
// 	}

	load(p.m_game_or_spawn,!xr_strcmp(p.m_new_or_load,"load") ? false : true, !xr_strcmp(p.m_new_or_load,"new"));
}

CALifeSimulator::~CALifeSimulator()
{
	VERIFY(!ai().get_alife());

	for (auto &it: m_configs_lru)
		FS.r_close(it.second);
}

void CALifeSimulator::destroy()
{
	CALifeUpdateManager::destroy();
	VERIFY						(ai().get_alife());
	ai().set_alife				(0);
}

void CALifeSimulator::kill_entity(CSE_ALifeMonsterAbstract *l_tpALifeMonsterAbstract, const u16 &l_tGraphID, CSE_ALifeSchedulable *schedulable)
{
	VERIFY(l_tpALifeMonsterAbstract->g_Alive());
	append_item_vector(l_tpALifeMonsterAbstract->children, m_temp_item_vector);

	u16 l_tGraphID1 = l_tpALifeMonsterAbstract->m_tGraphID;
	assign_death_position(l_tpALifeMonsterAbstract, l_tGraphID, schedulable);
	l_tpALifeMonsterAbstract->vfDetachAll();
	R_ASSERT(l_tpALifeMonsterAbstract->children.empty());

	scheduled().remove(l_tpALifeMonsterAbstract);

	if (l_tpALifeMonsterAbstract->m_tGraphID != l_tGraphID1) 
	{
		graph().remove(l_tpALifeMonsterAbstract, l_tGraphID1);
		graph().add(l_tpALifeMonsterAbstract, l_tpALifeMonsterAbstract->m_tGraphID);
	}
	CSE_ALifeInventoryItem *l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>(l_tpALifeMonsterAbstract);
	if (l_tpALifeInventoryItem)
		m_temp_item_vector.push_back(l_tpALifeInventoryItem);
}

void CALifeSimulator::setup_simulator(CSE_ALifeObject *object)
{
	object->m_alife_simulator	= this;
}

void CALifeSimulator::reload(LPCSTR section)
{
	CALifeUpdateManager::reload	(section);
}

struct string_prdicate
{
	shared_str	m_value;
	IC bool operator()(std::pair<shared_str, IReader*> const& value) const
	{
		return	!xr_strcmp(m_value, value.first);
	}
	IC string_prdicate(shared_str const& value) : m_value(value)
	{
	}
}; // struct string_prdicate

IReader const* CALifeSimulator::get_config(shared_str config) const
{
	configs_type::iterator const found = std::find_if(m_configs_lru.begin(), m_configs_lru.end(), string_prdicate(config));
	if (found != m_configs_lru.end()) {
		configs_type::value_type	temp = *found;
		m_configs_lru.erase(found);
		m_configs_lru.insert(m_configs_lru.begin(), std::make_pair(temp.first, temp.second));
		return						temp.second;
	}

	string_path						file_name;
	FS.update_path(file_name, "$game_config$", config.c_str());
	if (!FS.exist(file_name))
		return						0;

	m_configs_lru.insert(m_configs_lru.begin(), std::make_pair(config, FS.r_open(file_name)));
	return							m_configs_lru.front().second;
}

namespace detail
{

	bool object_exists_in_alife_registry(u32 id)
	{
		if (ai().get_alife())
		{
			return ai().alife().objects().object((ALife::_OBJECT_ID)id, true) != 0;
		}
		return false;
	}

} // detail
#include "alife_object_registry.h"
#include "alife_story_registry.h"
#include "script_engine.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "restriction_space.h"
#include "alife_graph_registry.h"
#include "alife_spawn_registry.h"
#include "alife_registry_container.h"
#include "xrServer.h"
#include "level.h"

using namespace luabind;

typedef xr_vector<std::pair<shared_str, int> >	STORY_PAIRS;
typedef STORY_PAIRS								SPAWN_STORY_PAIRS;
LPCSTR											_INVALID_STORY_ID = "INVALID_STORY_ID";
LPCSTR											_INVALID_SPAWN_STORY_ID = "INVALID_SPAWN_STORY_ID";
STORY_PAIRS										story_ids;
SPAWN_STORY_PAIRS								spawn_story_ids;

CALifeSimulator *alife()
{
	return			(const_cast<CALifeSimulator*>(ai().get_alife()));
}

CSE_ALifeDynamicObject *alife_object(const CALifeSimulator *self, ALife::_OBJECT_ID object_id)
{
	VERIFY(self);
	return			(self->objects().object(object_id, true));
}

bool valid_object_id(const CALifeSimulator *self, ALife::_OBJECT_ID object_id)
{
	VERIFY(self);
	return			(object_id != 0xffff);
}

#ifdef DEBUG
CSE_ALifeDynamicObject *alife_object(const CALifeSimulator *self, LPCSTR name)
{
	VERIFY(self);

	for (CALifeObjectRegistry::OBJECT_REGISTRY::const_iterator it = self->objects().objects().begin(); it != self->objects().objects().end(); it++) {
		CSE_ALifeDynamicObject	*obj = it->second;
		if (xr_strcmp(obj->name_replace(), name) == 0)
			return	(it->second);
	}

	return			(0);
}
#endif // #ifdef DEBUG

CSE_ALifeDynamicObject *alife_object(const CALifeSimulator *self, ALife::_OBJECT_ID id, bool no_assert)
{
	VERIFY(self);
	return			(self->objects().object(id, no_assert));
}

const xr_map<u16, CSE_ALifeDynamicObject*>& alife_objects(const CALifeSimulator *self)
{
	VERIFY(self);
	return self->objects().objects();
}

CSE_ALifeDynamicObject *alife_story_object(const CALifeSimulator *self, ALife::_STORY_ID id)
{
	return			(self->story_objects().object(id, true));
}

template <typename _id_type>
void generate_story_ids(
	STORY_PAIRS &result,
	_id_type	INVALID_ID,
	LPCSTR		section_name,
	LPCSTR		INVALID_ID_STRING,
	LPCSTR		invalid_id_description,
	LPCSTR		invalid_id_redefinition,
	LPCSTR		duplicated_id_description
)
{
	result.clear();

	CInifile				*Ini = pGameIni;

	LPCSTR					N, V;
	u32 					k;
	shared_str				temp;
	LPCSTR					section = section_name;
	R_ASSERT(Ini->section_exist(section));

	for (k = 0; Ini->r_line(section, k, &N, &V); ++k) {
		temp = Ini->r_string_wb(section, N);

		R_ASSERT3(!strchr(*temp, ' '), invalid_id_description, *temp);
		R_ASSERT2(xr_strcmp(*temp, INVALID_ID_STRING), invalid_id_redefinition);

		STORY_PAIRS::const_iterator	I = result.begin();
		STORY_PAIRS::const_iterator	E = result.end();
		for (; I != E; ++I)
			R_ASSERT3((*I).first != temp, duplicated_id_description, *temp);

		result.push_back(std::make_pair(*temp, atoi(N)));
	}

	result.push_back(std::make_pair(INVALID_ID_STRING, INVALID_ID));
}

void kill_entity0(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, const GameGraph::_GRAPH_ID &game_vertex_id)
{
	alife->kill_entity(monster, game_vertex_id, 0);
}

void kill_entity1(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster)
{
	alife->kill_entity(monster, monster->m_tGraphID, 0);
}

void add_in_restriction(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, ALife::_OBJECT_ID id)
{
	alife->add_restriction(monster->ID, id, RestrictionSpace::eRestrictorTypeIn);
}

void add_out_restriction(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, ALife::_OBJECT_ID id)
{
	alife->add_restriction(monster->ID, id, RestrictionSpace::eRestrictorTypeOut);
}

void remove_in_restriction(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, ALife::_OBJECT_ID id)
{
	alife->remove_restriction(monster->ID, id, RestrictionSpace::eRestrictorTypeIn);
}

void remove_out_restriction(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, ALife::_OBJECT_ID id)
{
	alife->remove_restriction(monster->ID, id, RestrictionSpace::eRestrictorTypeOut);
}

u32 get_level_id(CALifeSimulator *self)
{
	return						(self->graph().level().level_id());
}

CSE_ALifeDynamicObject *CALifeSimulator__create(CALifeSimulator *self, ALife::_SPAWN_ID spawn_id)
{
	const CALifeSpawnRegistry::SPAWN_GRAPH::CVertex	*vertex = ai().alife().spawns().spawns().vertex(spawn_id);
	THROW2(vertex, "Invalid spawn id!");

	CSE_ALifeDynamicObject				*spawn = smart_cast<CSE_ALifeDynamicObject*>(&vertex->data()->object());
	THROW(spawn);

	CSE_ALifeDynamicObject				*object;
	self->create(object, spawn, spawn_id);

	return								(object);
}

CSE_Abstract *CALifeSimulator__spawn_item(CALifeSimulator *self, LPCSTR section, const Fvector &position, u32 level_vertex_id, GameGraph::_GRAPH_ID game_vertex_id)
{
	THROW(self);
	return								(self->spawn_item(section, position, level_vertex_id, game_vertex_id, ALife::_OBJECT_ID(-1)));
}

CSE_Abstract *CALifeSimulator__spawn_item2(CALifeSimulator *self, LPCSTR section, const Fvector &position, u32 level_vertex_id, GameGraph::_GRAPH_ID game_vertex_id, ALife::_OBJECT_ID id_parent)
{
	if (id_parent == ALife::_OBJECT_ID(-1))
		return							(self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent));

	CSE_ALifeDynamicObject				*object = ai().alife().objects().object(id_parent, true);
	if (!object) {
		Msg("! invalid parent id [%d] specified", id_parent);
		return							(0);
	}

	if (!object->m_bOnline)
		return							(self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent));

	NET_Packet							packet;
	packet.w_begin(M_SPAWN);
	packet.w_stringZ(section);

	CSE_Abstract						*item = self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent, false);
	item->Spawn_Write(packet, FALSE);
	self->server().FreeID(item->ID, 0);
	F_entity_Destroy(item);

	ClientID							clientID;
	clientID.set(0xffff);

	u16									dummy;
	packet.r_begin(dummy);
	VERIFY(dummy == M_SPAWN);
	return								(self->server().Process_spawn(packet, clientID));
}

CSE_Abstract *CALifeSimulator__spawn_ammo(CALifeSimulator *self, LPCSTR section, const Fvector &position, u32 level_vertex_id, GameGraph::_GRAPH_ID game_vertex_id, ALife::_OBJECT_ID id_parent, int ammo_to_spawn)
{
	//	if (id_parent == ALife::_OBJECT_ID(-1))
	//		return							(self->spawn_item(section,position,level_vertex_id,game_vertex_id,id_parent));
	CSE_ALifeDynamicObject				*object = 0;
	if (id_parent != ALife::_OBJECT_ID(-1)) {
		object = ai().alife().objects().object(id_parent, true);
		if (!object) {
			Msg("! invalid parent id [%d] specified", id_parent);
			return						(0);
		}
	}

	if (!object || !object->m_bOnline) {
		CSE_Abstract					*item = self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent);

		CSE_ALifeItemAmmo				*ammo = smart_cast<CSE_ALifeItemAmmo*>(item);
		THROW(ammo);
		THROW(ammo->m_boxSize >= ammo_to_spawn);
		ammo->a_elapsed = (u16)ammo_to_spawn;

		return							(item);
	}

	NET_Packet							packet;
	packet.w_begin(M_SPAWN);
	packet.w_stringZ(section);

	CSE_Abstract						*item = self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent, false);

	CSE_ALifeItemAmmo					*ammo = smart_cast<CSE_ALifeItemAmmo*>(item);
	THROW(ammo);
	THROW(ammo->m_boxSize >= ammo_to_spawn);
	ammo->a_elapsed = (u16)ammo_to_spawn;

	item->Spawn_Write(packet, FALSE);
	self->server().FreeID(item->ID, 0);
	F_entity_Destroy(item);

	ClientID							clientID;
	clientID.set(0xffff);

	u16									dummy;
	packet.r_begin(dummy);
	VERIFY(dummy == M_SPAWN);
	return								(self->server().Process_spawn(packet, clientID));
}

ALife::_SPAWN_ID CALifeSimulator__spawn_id(CALifeSimulator *self, ALife::_SPAWN_STORY_ID spawn_story_id)
{
	return								(((const CALifeSimulator *)self)->spawns().spawn_id(spawn_story_id));
}

void CALifeSimulator__release(CALifeSimulator *self, CSE_Abstract *object, bool)
{
	VERIFY(self);
	//	self->release						(object,true);

	THROW(object);
	CSE_ALifeObject						*alife_object = smart_cast<CSE_ALifeObject*>(object);
	THROW(alife_object);
	if (!alife_object->m_bOnline) {
		self->release(object, true);
		return;
	}

	// awful hack, for stohe only
	NET_Packet							packet;
	packet.w_begin(M_EVENT);
	packet.w_u32(Level().timeServer());
	packet.w_u16(GE_DESTROY);
	packet.w_u16(object->ID);
	Level().Send(packet);
}

LPCSTR get_level_name(const CALifeSimulator *self, int level_id)
{
	LPCSTR								result = *ai().game_graph().header().level((GameGraph::_LEVEL_ID)level_id).name();
	return								(result);
}

CSE_ALifeCreatureActor *get_actor(const CALifeSimulator *self)
{
	THROW(self);
	return								(self->graph().actor());
}

KNOWN_INFO_VECTOR *registry(const CALifeSimulator *self, const ALife::_OBJECT_ID &id)
{
	THROW(self);
	return								(self->registry(info_portions).object(id, true));
}

bool has_info(const CALifeSimulator *self, const ALife::_OBJECT_ID &id, LPCSTR info_id)
{
	const KNOWN_INFO_VECTOR				*known_info = registry(self, id);
	if (!known_info || std::find_if(known_info->begin(), known_info->end(), CFindByIDPred(info_id)) == known_info->end())
		return false;
	else
		return true;
}

bool dont_has_info(const CALifeSimulator *self, const ALife::_OBJECT_ID &id, LPCSTR info_id)
{
	THROW(self);
	// absurdly, but only because of scriptwriters needs
	return								(!has_info(self, id, info_id));
}

#pragma optimize("s",on)
void CALifeSimulator::script_register(lua_State *L)
{
	module(L)
		[
		class_<CALifeSimulator>("alife_simulator")
			.def("valid_object_id", &valid_object_id)
			.def("level_id", &get_level_id)
			.def("level_name", &get_level_name)
			//.def("objects", &alife_objects, &return_stl_pair_iterator)
			.def("object", (CSE_ALifeDynamicObject *(*) (const CALifeSimulator *, ALife::_OBJECT_ID))(alife_object))
			.def("object", (CSE_ALifeDynamicObject *(*) (const CALifeSimulator *, ALife::_OBJECT_ID, bool))(alife_object))
			.def("story_object", (CSE_ALifeDynamicObject *(*) (const CALifeSimulator *, ALife::_STORY_ID))(alife_story_object))
			.def("set_switch_online", (void (CALifeSimulator::*) (ALife::_OBJECT_ID, bool))(&CALifeSimulator::set_switch_online))
			.def("set_switch_offline", (void (CALifeSimulator::*) (ALife::_OBJECT_ID, bool))(&CALifeSimulator::set_switch_offline))
			.def("set_interactive", (void (CALifeSimulator::*) (ALife::_OBJECT_ID, bool))(&CALifeSimulator::set_interactive))
			.def("kill_entity", &CALifeSimulator::kill_entity)
			.def("kill_entity", &kill_entity0)
			.def("kill_entity", &kill_entity1)
			.def("add_in_restriction", &add_in_restriction)
			.def("add_out_restriction", &add_out_restriction)
			.def("remove_in_restriction", &remove_in_restriction)
			.def("remove_out_restriction", &remove_out_restriction)
			.def("remove_all_restrictions", &CALifeSimulator::remove_all_restrictions)
			.def("create", &CALifeSimulator__create)
			.def("create", &CALifeSimulator__spawn_item2)
			.def("create", &CALifeSimulator__spawn_item)
			.def("create_ammo", &CALifeSimulator__spawn_ammo)
			.def("release", &CALifeSimulator__release)
			.def("spawn_id", &CALifeSimulator__spawn_id)
			.def("actor", &get_actor)
			.def("has_info", &has_info)
			.def("dont_has_info", &dont_has_info)
			.def("switch_distance", &CALifeSimulator::switch_distance)
			.def("set_switch_distance", &CALifeSimulator::set_switch_distance)
			
			, def("alife", &alife)
		];

	{
		if (story_ids.empty())
			generate_story_ids(
				story_ids,
				INVALID_STORY_ID,
				"story_ids",
				"INVALID_STORY_ID",
				"Invalid story id description (contains spaces)!",
				"INVALID_STORY_ID redifinition!",
				"Duplicated story id description!"
			);

		luabind::class_<class_exporter<CALifeSimulator> >	instance("story_ids");

		for (const auto& pair : story_ids)
			instance = std::move(instance).enum_("_story_ids")[luabind::value(*pair.first, pair.second)];

		luabind::module(L)[std::move(instance)];
	}

	{
		if (spawn_story_ids.empty())
			generate_story_ids(spawn_story_ids, INVALID_SPAWN_STORY_ID, "spawn_story_ids", "INVALID_SPAWN_STORY_ID", "Invalid spawn story id description (contains spaces)!", "INVALID_SPAWN_STORY_ID redifinition!", "Duplicated spawn story id description!");

		luabind::class_<class_exporter<class_exporter<CALifeSimulator> > >	instance("spawn_story_ids");

		for (const auto& pair : spawn_story_ids)
			instance = std::move(instance).enum_("_spawn_story_ids")[luabind::value(*pair.first, pair.second)];

		luabind::module(L)[std::move(instance)];
	}
}
