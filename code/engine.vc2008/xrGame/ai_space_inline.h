////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space_inline.h
//	Created 	: 12.11.2003
//  Modified 	: 25.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CGameGraph &CAI_Space::game_graph() const
{
	return *m_game_graph;
}

IC CLevelGraph &CAI_Space::level_graph() const
{
	return *m_level_graph;
}

IC const CLevelGraph *CAI_Space::get_level_graph() const
{
	return m_level_graph.get();
}

IC CEF_Storage &CAI_Space::ef_storage() const
{
	return *m_ef_storage;
}

IC CGraphEngine &CAI_Space::graph_engine() const
{
	return *m_graph_engine;
}

IC const CALifeSimulator &CAI_Space::alife() const
{
	VERIFY(m_alife_simulator);
	return *m_alife_simulator;
}

IC const CALifeSimulator *CAI_Space::get_alife() const
{
	return m_alife_simulator;
}

IC const CCoverManager &CAI_Space::cover_manager() const
{
	return *m_cover_manager;
}

IC CScriptEngine &CAI_Space::script_engine() const
{
	VERIFY(m_script_engine);
	return *m_script_engine;
}

IC const CPatrolPathStorage &CAI_Space::patrol_paths() const
{
	return *m_patrol_path_storage;
}

IC moving_objects &CAI_Space::moving_objects() const
{
	return *m_moving_objects;
}

IC doors::manager& CAI_Space::doors() const
{
	return *m_doors_manager;
}

IC CPatrolPathStorage &CAI_Space::patrol_paths_raw()
{
	return *m_patrol_path_storage;
}
