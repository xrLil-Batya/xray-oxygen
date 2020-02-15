////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_storage.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path storage
////////////////////////////////////////////////////////////////////////////

#pragma once

class CPatrolPath;
class CLevelGraph;
class CGameLevelCrossTable;
class CGameGraph;

#include "object_interfaces.h"
#include "associative_vector.h"

class GAME_API CPatrolPathStorage : public IPureSerializeObject<IReader,IWriter> 
{
	typedef IPureSerializeObject<IReader,IWriter>		inherited;

public:
	typedef associative_vector<shared_str,CPatrolPath*>	PATROL_REGISTRY;
	typedef PATROL_REGISTRY::iterator					iterator;
	typedef PATROL_REGISTRY::const_iterator				const_iterator;

protected:
	PATROL_REGISTRY					m_registry;

public:
	constexpr						CPatrolPathStorage	() {};
	virtual							~CPatrolPathStorage	();
	virtual	void					load				(IReader &stream);
	virtual	void					save				(IWriter &stream);

public:
			void load_raw (const CLevelGraph *level_graph, const CGameLevelCrossTable *cross, IReader &stream);
	IC		const CPatrolPath		*path				(shared_str patrol_name, bool no_assert = false) const { return (I == patrol_paths().end()) ? nullptr : ((*I).second); };
	IC		const PATROL_REGISTRY	&patrol_paths		() const { return m_registry; };

			void					add_path(shared_str patrol_name, CPatrolPath *path);
			void					remove_path(shared_str patrol_name);
};