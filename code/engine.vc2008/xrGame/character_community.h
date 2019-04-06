//////////////////////////////////////////////////////////////////////////
// character_community.h:	структура представления группировки
//							
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "ini_id_loader.h"
#include "ini_table_loader.h"

#include "character_info_defs.h"


struct COMMUNITY_DATA
{
	COMMUNITY_DATA (int, shared_str, LPCSTR);

	u8			team;
	int			index;
	shared_str	id;
};

class GAME_API CCharacterCommunity : public CIni_IdToIndex<true, COMMUNITY_DATA, shared_str, int, CCharacterCommunity>
{
	typedef CIni_IdToIndex<true, COMMUNITY_DATA, shared_str, int, CCharacterCommunity> inherited;
	friend inherited;

public:
	CCharacterCommunity			();
	~CCharacterCommunity		();

	void						set				(shared_str);		
	void						set				(int index) {m_current_index = index;};

	shared_str					id				() const;
	int							index			() const	{return m_current_index;};
	u8							team			() const;

private:
	int	m_current_index;

	static	void				InitIdToIndex	();

public:
	//отношение между группировками
	static CHARACTER_GOODWILL	relation			(int from, int to);
	CHARACTER_GOODWILL			relation			(int to);
	
	static void					set_relation		(int from, int to, CHARACTER_GOODWILL goodwill);

	static float				sympathy			(int);
	
	static void					DeleteIdToIndexData	();
private:
	typedef CIni_Table<CHARACTER_GOODWILL, CCharacterCommunity> GOODWILL_TABLE;
	friend GOODWILL_TABLE;
	static GOODWILL_TABLE m_relation_table;

	//таблица коэффициентов "сочуствия" между участниками группировки
	typedef CIni_Table<float, CCharacterCommunity> SYMPATHY_TABLE;
	friend SYMPATHY_TABLE;
	static SYMPATHY_TABLE m_sympathy_table;
};