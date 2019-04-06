//////////////////////////////////////////////////////////////////////////
// character_community.cpp:		структура представления группировки
//							
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_community.h"

//////////////////////////////////////////////////////////////////////////
COMMUNITY_DATA::COMMUNITY_DATA (int idx, shared_str idn, LPCSTR team_str)
{
	index = idx;
	id = idn;
	team = (u8)atoi_17(team_str);
}

//////////////////////////////////////////////////////////////////////////
CCharacterCommunity::GOODWILL_TABLE CCharacterCommunity::m_relation_table;
CCharacterCommunity::SYMPATHY_TABLE CCharacterCommunity::m_sympathy_table;

//////////////////////////////////////////////////////////////////////////
CCharacterCommunity::CCharacterCommunity	()
{
	m_current_index = NO_COMMUNITY_INDEX;
}
CCharacterCommunity::~CCharacterCommunity	()
{
}


void  CCharacterCommunity::set	(shared_str id)
{
	m_current_index	 = IdToIndex(id);

}

shared_str		 CCharacterCommunity::id			() const
{
	return IndexToId(m_current_index);
}

u8							 CCharacterCommunity::team			() const
{
	return (*m_pItemDataVector)[m_current_index].team;
}


void CCharacterCommunity::InitIdToIndex	()
{
	section_name = "game_relations";
	line_name = "communities";

	m_relation_table.set_table_params("communities_relations");
	m_sympathy_table.set_table_params("communities_sympathy", 1);
}


CHARACTER_GOODWILL CCharacterCommunity::relation		(int to)
{
	return relation(m_current_index, to);
}

CHARACTER_GOODWILL  CCharacterCommunity::relation		(int from, int to)
{
	VERIFY(from >= 0 && from <(int)m_relation_table.table().size());
	VERIFY(to >= 0 && to <(int)m_relation_table.table().size());
	
	return m_relation_table.table()[from][to];
}

void  CCharacterCommunity::set_relation			(int from, int to, CHARACTER_GOODWILL goodwill)
{
	VERIFY(from >= 0 && from <(int)m_relation_table.table().size());
	VERIFY(to >= 0 && to <(int)m_relation_table.table().size());
	VERIFY(goodwill != NO_GOODWILL);

	m_relation_table.table()[from][to] = goodwill;
}

float  CCharacterCommunity::sympathy			(int comm)
{
	VERIFY(comm >= 0 && comm <(int)m_sympathy_table.table().size());
	return m_sympathy_table.table()[comm][0];
}

void CCharacterCommunity::DeleteIdToIndexData	()
{
	m_relation_table.clear();
	m_sympathy_table.clear();
	inherited::DeleteIdToIndexData();
}