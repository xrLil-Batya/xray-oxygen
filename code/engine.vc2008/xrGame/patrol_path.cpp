////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_path.h"
#include "levelgamedef.h"

LPCSTR TEST_PATROL_PATH_NAME = "val_dogs_nest4_centre";

CPatrolPath::CPatrolPath(shared_str name)
{
#ifdef DEBUG
	m_name = name;
#endif
}

CPatrolPath::~CPatrolPath()
{
}

CPatrolPath& CPatrolPath::load_raw(const CLevelGraph *level_graph, const CGameLevelCrossTable *cross, IReader &stream)
{
	R_ASSERT(stream.find_chunk(WAYOBJECT_CHUNK_POINTS));
	u32	vertex_count = stream.r_u16();
	for (u32 i = 0; i < vertex_count; ++i)
		add_vertex(CPatrolPoint(this).load_raw(level_graph, cross, stream), i);

	R_ASSERT(stream.find_chunk(WAYOBJECT_CHUNK_LINKS));
	u32	edge_count = stream.r_u16();
	for (u32 i = 0; i < edge_count; ++i) {
		u16	vertex0 = stream.r_u16();
		u16	vertex1 = stream.r_u16();
		float probability = stream.r_float();
		add_edge(vertex0, vertex1, probability);
	}

	return *this;
}

#ifdef DEBUG
void CPatrolPath::load(IReader &stream)
{
	inherited::load(stream);
	vertex_iterator	I = vertices().begin();
	vertex_iterator	E = vertices().end();
	for (; I != E; ++I)
		(*I).second->data().path(this);
}
#endif

CPatrolPoint CPatrolPath::add_point(Fvector position)
{
	auto pp = CPatrolPoint(this).position(position);
	u32 index = (u32)vertices().size();
	add_vertex(pp, index);
	return vertex(index)->data();
}

CPatrolPoint CPatrolPath::point(u32 index)
{
	return vertex(index)->data();
}