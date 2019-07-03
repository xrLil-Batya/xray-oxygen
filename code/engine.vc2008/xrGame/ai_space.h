////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space.h
//	Created 	: 12.11.2003
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "game_graph.h"
#include "level_graph.h"
#include "ef_storage.h"
#include "doors_manager.h"
#include "patrol_path_manager.h"
#include "cover_manager.h"
#include "moving_objects.h"
#include "script_engine.h"
#include "alife_simulator.h"
#include "graph_engine.h"

class CGameLevelCrossTable;
class CGraphEngine;

class GAME_API CAI_Space 
{
	friend class CALifeSimulator;
	friend class CALifeGraphRegistry;
	friend class CALifeSpawnRegistry;
	friend class CALifeSpawnRegistry;
	friend class CLevel;

	xrScopePtr<CGameGraph			>			m_game_graph;
	xrScopePtr<CLevelGraph			>			m_level_graph;
	xrScopePtr<CGraphEngine			>			m_graph_engine;
	xrScopePtr<CEF_Storage			>			m_ef_storage;
	xrScopePtr<CCoverManager		>			m_cover_manager;
	xrScopePtr<CPatrolPathStorage	>			m_patrol_path_storage;
	xrScopePtr<moving_objects		>			m_moving_objects;
	xrScopePtr<doors::manager		>			m_doors_manager;

	CALifeSimulator* m_alife_simulator = nullptr;
	CScriptEngine*   m_script_engine   = nullptr;
private:
			void						load					(LPCSTR level_name);
			void						unload					(bool reload = false);
			void						patrol_path_storage_raw	(IReader &stream);
			void						patrol_path_storage		(IReader &stream);
			void						set_alife				(CALifeSimulator *alife_simulator);
			void create_game_graph (IReader& chunk);

public:
	CAI_Space				();

	static CAI_Space&					GetInstance				();
	virtual								~CAI_Space				();
			void						init					();
	IC		CGameGraph					&game_graph				() const;
	IC		bool						is_game_graph_presented () const;
	IC		CLevelGraph					&level_graph			() const;
	IC		const CLevelGraph			*get_level_graph		() const;
			const CGameLevelCrossTable	&cross_table			() const;
			const CGameLevelCrossTable	*get_cross_table		() const;
	IC		const CPatrolPathStorage	&patrol_paths			() const;
	IC		CEF_Storage					&ef_storage				() const;
	IC		CGraphEngine				&graph_engine			() const;
	IC		const CALifeSimulator		&alife					() const;
	IC		const CALifeSimulator		*get_alife				() const;
	IC		const CCoverManager			&cover_manager			() const;
	IC		CScriptEngine				&script_engine			() const;
	IC		moving_objects				&moving_objects			() const;
	IC		doors::manager&				doors					() const;
	IC		CPatrolPathStorage			&patrol_paths_raw		();

#ifdef DEBUG
			void						validate				(const u32			level_id) const;
#endif
};

IC CAI_Space &ai()
{
	return CAI_Space::GetInstance();
}

#include "ai_space_inline.h"