//////////////////////////////////////////////////////////////////////////
// relation_registry.cpp:	реестр для хранения данных об отношении персонажа к 
//							другим персонажам
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "relation_registry.h"
#include "alife_registry_wrappers.h"

#include "character_community.h"
#include "character_reputation.h"
#include "character_rank.h"

#include "alife_object_registry.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_engine.h"


//////////////////////////////////////////////////////////////////////////

SRelation::SRelation()
{
	m_iGoodwill = NEUTRAL_GOODWILL;
}

SRelation::~SRelation()
{
}

//////////////////////////////////////////////////////////////////////////

void RELATION_DATA::clear	()
{
	personal.clear();
	communities.clear();
}

void RELATION_DATA::load (IReader& stream)
{
	load_data(personal, stream);
	load_data(communities, stream);
}

void RELATION_DATA::save (IWriter& stream)
{
	save_data(personal, stream);
	save_data(communities, stream);
}

//////////////////////////////////////////////////////////////////////////

SRelationRegistry::RELATION_MAP_SPOTS::RELATION_MAP_SPOTS()
{
	spot_names[ALife::eRelationTypeFriend]		= "friend_location";
	spot_names[ALife::eRelationTypeNeutral]		= "neutral_location";
	spot_names[ALife::eRelationTypeEnemy]		= "enemy_location";
	spot_names[ALife::eRelationTypeWorstEnemy]	= "enemy_location";
	spot_names[ALife::eRelationTypeLast]		= "neutral_location";
}
//////////////////////////////////////////////////////////////////////////

CRelationRegistryWrapper*					SRelationRegistry::m_relation_registry	= nullptr;
SRelationRegistry::FIGHT_VECTOR*			SRelationRegistry::m_fight_registry		= nullptr;
SRelationRegistry::RELATION_MAP_SPOTS*		SRelationRegistry::m_spot_names			= nullptr;


//////////////////////////////////////////////////////////////////////////


SRelationRegistry::SRelationRegistry  ()
{
}

SRelationRegistry::~SRelationRegistry ()
{
}

//////////////////////////////////////////////////////////////////////////

extern void load_attack_goodwill();
CRelationRegistryWrapper& SRelationRegistry::relation_registry()
{
	if(!m_relation_registry){
		m_relation_registry = xr_new<CRelationRegistryWrapper>();
		load_attack_goodwill();
	}

	return *m_relation_registry;
}


SRelationRegistry::FIGHT_VECTOR& SRelationRegistry::fight_registry()
{
	if(!m_fight_registry)
		m_fight_registry = xr_new<FIGHT_VECTOR>();

	return *m_fight_registry;
}

void SRelationRegistry::clear_relation_registry()
{
	xr_delete(m_relation_registry);
	xr_delete(m_fight_registry);
	xr_delete(m_spot_names);
}

const shared_str& SRelationRegistry::GetSpotName(ALife::ERelationType& type)
{
	if(!m_spot_names)
		m_spot_names = xr_new<RELATION_MAP_SPOTS>();
	return m_spot_names->GetSpotName(type);
}

//////////////////////////////////////////////////////////////////////////

void SRelationRegistry::ClearRelations	(u16 person_id)
{
	const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(person_id);
	if(relation_data)
	{
		relation_registry().registry().objects(person_id).clear();
	}
}



//////////////////////////////////////////////////////////////////////////
CHARACTER_GOODWILL	 SRelationRegistry::GetGoodwill			(u16 from, u16 to) const 
{
	const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(from);

	if(relation_data)
	{
		PERSONAL_RELATION_MAP::const_iterator it = relation_data->personal.find(to);
		if(relation_data->personal.end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill();
		}
	}
	//если отношение еще не задано, то возвращаем нейтральное
	return NEUTRAL_GOODWILL;
}

void SRelationRegistry::SetGoodwill 	(u16 from, u16 to, CHARACTER_GOODWILL goodwill)
{
	RELATION_DATA& relation_data = relation_registry().registry().objects(from);

	static Ivector2 gw_limits		= pSettings->r_ivector2(ACTIONS_POINTS_SECT, "personal_goodwill_limits");
	clamp							(goodwill, gw_limits.x, gw_limits.y);

	relation_data.personal[to].SetGoodwill(goodwill);
}

void SRelationRegistry::ForceSetGoodwill 	(u16 from, u16 to, CHARACTER_GOODWILL goodwill)
{
	RELATION_DATA& relation_data = relation_registry().registry().objects(from);

	CSE_ALifeTraderAbstract* from_obj	= smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(from));
	CSE_ALifeTraderAbstract* to_obj		= smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(to));
	
	if (!from_obj||!to_obj){
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"SRelationRegistry::ForceSetGoodwill  : cannot convert obj to CSE_ALifeTraderAbstract!");
		return;
	}	
	CHARACTER_GOODWILL community_to_obj_goodwill		= GetCommunityGoodwill	(from_obj->Community(), to					);
	CHARACTER_GOODWILL community_to_community_goodwill	= GetCommunityRelation	(from_obj->Community(), to_obj->Community()	);
	
	relation_data.personal[to].SetGoodwill(goodwill - community_to_obj_goodwill - community_to_community_goodwill);
}


void SRelationRegistry::ChangeGoodwill 	(u16 from, u16 to, CHARACTER_GOODWILL delta_goodwill)
{
	CHARACTER_GOODWILL new_goodwill		= GetGoodwill(from, to)+ delta_goodwill;
	SetGoodwill							(from, to, new_goodwill);
}

//////////////////////////////////////////////////////////////////////////
CHARACTER_GOODWILL	 SRelationRegistry::GetCommunityGoodwill (int from_community, u16 to_character) const 
{
	const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(to_character);

	if(relation_data)
	{
		COMMUNITY_RELATION_MAP::const_iterator it = relation_data->communities.find(from_community);
		if(relation_data->communities.end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill();
		}
	}
	//если отношение еще не задано, то возвращаем нейтральное
	return NEUTRAL_GOODWILL;
}

void SRelationRegistry::SetCommunityGoodwill 	(int from_community, u16 to_character, CHARACTER_GOODWILL goodwill)
{
	static Ivector2 gw_limits		= pSettings->r_ivector2(ACTIONS_POINTS_SECT, "community_goodwill_limits");
	clamp							(goodwill, gw_limits.x, gw_limits.y);
	RELATION_DATA& relation_data	= relation_registry().registry().objects(to_character);

	relation_data.communities[from_community].SetGoodwill(goodwill);
}

void SRelationRegistry::ChangeCommunityGoodwill (int from_community, u16 to_character, CHARACTER_GOODWILL delta_goodwill)
{
	CHARACTER_GOODWILL gw = GetCommunityGoodwill(from_community, to_character)+ delta_goodwill;
	SetCommunityGoodwill	(from_community, to_character, gw);
}
//////////////////////////////////////////////////////////////////////////

CHARACTER_GOODWILL	 SRelationRegistry::GetCommunityRelation		(int index1, int index2) const
{
	return CCharacterCommunity::relation(index1, index2);
}

CHARACTER_GOODWILL	 SRelationRegistry::GetRankRelation				(CHARACTER_RANK_VALUE rank1, CHARACTER_RANK_VALUE rank2) const
{
	CHARACTER_RANK rank_from, rank_to;
	rank_from.set(rank1);
	rank_to.set(rank2);
	return CHARACTER_RANK::relation(rank_from.index(), rank_to.index());
}

CHARACTER_GOODWILL	 SRelationRegistry::GetReputationRelation		(CHARACTER_REPUTATION_VALUE rep1, CHARACTER_REPUTATION_VALUE rep2) const
{
	CHARACTER_REPUTATION rep_from, rep_to;
	rep_from.set(rep1);
	rep_to.set(rep2);
	return CHARACTER_REPUTATION::relation(rep_from.index(), rep_to.index());
}

//////////////////////////////////////////////////////////////////////////

void SRelationRegistry::SetCommunityRelation( int index1, int index2, CHARACTER_GOODWILL goodwill )
{
	CCharacterCommunity::set_relation( index1, index2, goodwill );
}
