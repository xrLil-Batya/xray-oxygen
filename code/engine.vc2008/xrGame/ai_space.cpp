////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space.h
//	Created 	: 12.11.2003
//  Modified 	: 06.04.2017
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_graph.h"
#include "game_level_cross_table.h"
#include "level_graph.h"
#include "graph_engine.h"
#include "ef_storage.h"
#include "ai_space.h"
#include "cover_manager.h"
#include "cover_point.h"
#include "script_engine.h"
#include "patrol_path_storage.h"
#include "alife_simulator.h"
#include "moving_objects.h"
#include "doors_manager.h"
#include "../FrayBuildConfig.hpp"
#include <memory>

static CAI_Space* g_ai_space = nullptr;

CAI_Space& CAI_Space::GetInstance()
{
	if (!g_ai_space)
	{
		g_ai_space = new CAI_Space();
		g_ai_space->init();
	}

	return *g_ai_space;
}

CAI_Space::CAI_Space()
{
	m_ef_storage = nullptr;
	m_game_graph = nullptr;
	m_graph_engine = nullptr;
	m_cover_manager = nullptr;
	m_level_graph = nullptr;
	m_alife_simulator = nullptr;
	m_patrol_path_storage = nullptr;
	m_script_engine = nullptr;
	m_moving_objects = nullptr;
	m_doors_manager = nullptr;
}

void CAI_Space::init()
{
	m_ef_storage = std::make_unique<CEF_Storage>();
	m_cover_manager = std::make_unique<CCoverManager>();
	m_moving_objects = std::make_unique<::moving_objects>();

	m_graph_engine = std::make_unique<CGraphEngine>(1024);
	m_patrol_path_storage = std::make_unique<CPatrolPathStorage>();
	m_script_engine = new CScriptEngine();
	script_engine().init();

}

CAI_Space::~CAI_Space()
{
	unload();
	xr_delete(m_script_engine);
}

void CAI_Space::load(LPCSTR level_name)
{
	VERIFY(m_game_graph);

	unload(true);

#ifdef DEBUG
	Memory.mem_compact();
	u32						mem_usage = Memory.mem_usage();
	CTimer					timer;
	timer.Start();
#endif

	const CGameGraph::SLevel& current_level = game_graph().header().level(level_name);

	m_level_graph = std::make_unique<CLevelGraph>();
	game_graph().set_current_level(current_level.id());
	R_ASSERT2(cross_table().header().level_guid() == level_graph().header().guid(), "cross_table doesn't correspond to the AI-map");
	R_ASSERT2(cross_table().header().game_guid() == game_graph().header().guid(), "graph doesn't correspond to the cross table");
	m_graph_engine = std::make_unique<CGraphEngine>(std::max((u32)game_graph().header().vertex_count(), level_graph().header().vertex_count()));
	R_ASSERT2(current_level.guid() == level_graph().header().guid(), "graph doesn't correspond to the AI-map");

#ifdef DEBUG
	if (!xr_strcmp(current_level.name(), level_name))
		validate(current_level.id());
#endif

	level_graph().level_id(current_level.id());
	m_cover_manager->compute_static_cover();
	m_moving_objects->on_level_load();

	VERIFY(!m_doors_manager);
	m_doors_manager = std::make_unique<::doors::manager>(level_graph().header().box());
}

void CAI_Space::unload(bool reload)
{
	script_engine().unload();

	m_doors_manager.reset(nullptr);
	m_graph_engine.reset(nullptr);
	m_level_graph.reset(nullptr);

	if (!reload && m_game_graph)
		m_graph_engine = std::make_unique<CGraphEngine>(game_graph().header().vertex_count());
}

#ifdef DEBUG
void CAI_Space::validate(const u32 level_id) const
{
	VERIFY(level_graph().header().vertex_count() == cross_table().header().level_vertex_count());
	for (GameGraph::_GRAPH_ID i = 0, n = game_graph().header().vertex_count(); i < n; ++i)
		if ((level_id == game_graph().vertex(i)->level_id()) &&
			(!level_graph().valid_vertex_id(game_graph().vertex(i)->level_vertex_id()) ||
			(cross_table().vertex(game_graph().vertex(i)->level_vertex_id()).game_vertex_id() != i) ||
				!level_graph().inside(game_graph().vertex(i)->level_vertex_id(), game_graph().vertex(i)->level_point()))) {
			Msg("! Graph doesn't correspond to the cross table");
			R_ASSERT2(false, "Graph doesn't correspond to the cross table");
		}

	//	Msg						("death graph point id : %d",cross_table().vertex(455236).game_vertex_id());

	for (u32 i = 0, n = game_graph().header().vertex_count(); i < n; ++i) {
		if (level_id != game_graph().vertex(i)->level_id())
			continue;

		CGameGraph::const_spawn_iterator	I, E;
		game_graph().begin_spawn(i, I, E);
		//		Msg									("vertex [%d] has %d death points",i,game_graph().vertex(i)->death_point_count());
		for (; I != E; ++I) {
			VERIFY(cross_table().vertex((*I).level_vertex_id()).game_vertex_id() == i);
		}
	}


	//	Msg						("* Graph corresponds to the cross table");
}
#endif

void CAI_Space::patrol_path_storage_raw(IReader& stream)
{
	m_patrol_path_storage = std::make_unique<CPatrolPathStorage>();
	m_patrol_path_storage->load_raw(get_level_graph(), get_cross_table(), stream);
}

void CAI_Space::patrol_path_storage(IReader& stream)
{
	m_patrol_path_storage = std::make_unique<CPatrolPathStorage>();
	m_patrol_path_storage->load(stream);
}

void CAI_Space::set_alife(CALifeSimulator* alife_simulator)
{
	m_alife_simulator = alife_simulator;

	if (alife_simulator)
		return;

	m_game_graph = nullptr;
	//	m_graph_engine.reset(nullptr);
}

void CAI_Space::create_game_graph(IReader& chunk)
{
	VERIFY(m_alife_simulator);
	VERIFY(!m_game_graph);
	VERIFY(!m_game_graph);

	m_game_graph = std::make_unique<CGameGraph>(chunk);
	m_graph_engine = std::make_unique<CGraphEngine>(this->game_graph().header().vertex_count());
}

IC bool CAI_Space::is_game_graph_presented() const
{
	return static_cast<bool> (m_game_graph);
}

const CGameLevelCrossTable& CAI_Space::cross_table() const
{
	return (game_graph().cross_table());
}

const CGameLevelCrossTable* CAI_Space::get_cross_table() const
{
	return (&game_graph().cross_table());
}