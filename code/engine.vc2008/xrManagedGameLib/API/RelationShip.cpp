#include "stdafx.h"
#include "RelationShip.h"
#include "../xrGame/ai_space.h"
#include "../xrGame/character_community.h"
#include "../xrGame/alife_simulator.h"
#include "../xrGame/alife_object_registry.h"
#include "../xrServerEntities/xrServer_Objects_ALife_Monsters.h"
#include "../xrGame/relation_registry.h"
#include "../xrServerEntities/script_storage.h"
#include "../xrGame/relation_registry.h"

XRay::RelationShip::RelationShip(::System::String^ _community, int _entity_id) : EntityId((u32)_entity_id), LastRSVal(0)
{
	string64 SCommunityName = { 0 };
	ConvertDotNetStringToAscii(_community, SCommunityName);
	CommunityName = SCommunityName;

	pCommunity = new CCharacterCommunity();
}

int XRay::RelationShip::CommunityRelationShip::get()
{
	pCommunity->set(CommunityName);
	return SRelationRegistry().GetCommunityGoodwill(pCommunity->index(), u16(EntityId));
}

void XRay::RelationShip::CommunityRelationShip::set(int val)
{
	LastRSVal = val;
	pCommunity->set(CommunityName);
	SRelationRegistry().SetCommunityGoodwill(pCommunity->index(), u16(EntityId), LastRSVal);
}

void XRay::RelationShip::Change()
{
	SRelationRegistry().ChangeCommunityGoodwill(pCommunity->index(), u16(EntityId), LastRSVal);
}

int XRay::RelationShip::GetCommunityRelation(::System::String^ comm_from, ::System::String^ comm_to)
{
	// Cast strings
	string64 SCommunityName1 = { 0 };
	string64 SCommunityName2 = { 0 };
	ConvertDotNetStringToAscii(comm_from, SCommunityName1);
	ConvertDotNetStringToAscii(comm_to, SCommunityName2);

	// Make
	CCharacterCommunity	community_from;
	community_from.set(SCommunityName1);
	CCharacterCommunity	community_to;
	community_to.set(SCommunityName2);

	return SRelationRegistry().GetCommunityRelation(community_from.index(), community_to.index());
}

void XRay::RelationShip::SetCommunityRelation(::System::String^ comm_from, ::System::String^ comm_to, int value)
{
	// Cast strings
	string64 SCommunityName1 = { 0 };
	string64 SCommunityName2 = { 0 };
	ConvertDotNetStringToAscii(comm_from, SCommunityName1);
	ConvertDotNetStringToAscii(comm_to, SCommunityName2);

	// Make
	CCharacterCommunity	community_from;
	community_from.set(SCommunityName1);
	CCharacterCommunity	community_to;
	community_to.set(SCommunityName2);

	SRelationRegistry().SetCommunityRelation(community_from.index(), community_to.index(), value);
}

int XRay::RelationShip::GetGeneralGoodwillBetween(u16 from, u16 to)
{
	CHARACTER_GOODWILL presonal_goodwill = SRelationRegistry().GetGoodwill(from, to); VERIFY(presonal_goodwill != NO_GOODWILL);

	CSE_ALifeTraderAbstract* from_obj = smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(from));
	CSE_ALifeTraderAbstract* to_obj = smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(to));

	if (!from_obj || !to_obj)
	{
		Msg("SRelationRegistry::get_general_goodwill_between  : cannot convert obj to CSE_ALifeTraderAbstract!");
		return (0);
	}
	CHARACTER_GOODWILL community_to_obj_goodwill = SRelationRegistry().GetCommunityGoodwill(from_obj->Community(), to);
	CHARACTER_GOODWILL community_to_community_goodwill = SRelationRegistry().GetCommunityRelation(from_obj->Community(), to_obj->Community());

	return presonal_goodwill + community_to_obj_goodwill + community_to_community_goodwill;
}