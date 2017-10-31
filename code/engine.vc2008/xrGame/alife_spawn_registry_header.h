////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry_header.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry header
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xrEngine/xrLevel.h"

class CALifeSpawnHeader 
{
protected:
	u32						m_version;
	xrGUID					m_guid;
	xrGUID					m_graph_guid;
	u32						m_count;
	u32						m_level_count;

public:
	virtual					~CALifeSpawnHeader	() = default;
	virtual void			load				(IReader &file_stream);
	IC		u32				version				() const { return (m_version); }
	IC		const xrGUID	&guid				() const { return (m_guid); }
	IC		const xrGUID	&graph_guid			() const { return (m_graph_guid); }
	IC		u32				count				() const { return (m_count); }
	IC		u32				level_count			() const { return (m_level_count); }
};
