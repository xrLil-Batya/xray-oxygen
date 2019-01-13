////////////////////////////////////////////////////////////////////////////
//	Module 		: item_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Item manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "item_manager.h"
#include "inventory_item.h"
#include "custommonster.h"
#include "ai_object_location.h"
#include "level_graph.h"
#include "restricted_object.h"
#include "ai_space.h"
#include "../xrEngine/profiler.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_movement_manager_smart_cover.h"
#include "items/Weapon.h"

CItemManager::CItemManager(CCustomMonster *object)
{
	VERIFY(object);
	m_object = object;
	m_stalker = smart_cast<CAI_Stalker*>( m_object );
}

bool CItemManager::is_useful(const CGameObject *object) const
{
	return m_object->useful(this,object);
}

bool CItemManager::useful(const CGameObject *object) const
{
	if (!inherited::is_useful(object))
		return false;

	if (m_object->getDestroy())
		return false;

	// we do not want to keep in memory attached objects
	if (m_object->H_Parent())
		return false;

	if (!const_cast<CGameObject*>(object)->UsedAI_Locations())
		return false;

	if (!m_object->movement().restrictions().accessible(object->Position()))
		return false;

	if (!m_object->movement().restrictions().accessible(object->ai_location().level_vertex_id()))
		return false;

	// ForserX: NPC can only raise a weapon
	const CWeapon* isWeapon = smart_cast<const CWeapon*>(object);
	if (!isWeapon) return false;

	const CInventoryItem *inventory_item = smart_cast<const CInventoryItem*>(object);
	

	if (inventory_item && !inventory_item->useful_for_NPC())
		return false;

	if (m_stalker && !m_stalker->movement().restrictions().accessible(inventory_item->object().Position()))
		return false;

	if (!ai().get_level_graph())
		return false;

	if (!ai().level_graph().valid_vertex_id(object->ai_location().level_vertex_id()))
		return false;

	if (!ai().level_graph().inside(object->ai_location().level_vertex_id(), inventory_item->object().Position()))
		return false;

	return true;
}

float CItemManager::do_evaluate(const CGameObject *object) const
{
	VERIFY3(m_object->movement().restrictions().accessible(object->ai_location().level_vertex_id()), *m_object->cName(), *object->cName());
	return (m_object->evaluate(this,object));
}

float CItemManager::evaluate(const CGameObject *object) const
{
	const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(object);
	VERIFY					(inventory_item);
	VERIFY					(inventory_item->useful_for_NPC());
	return					(1000000.f - (float)inventory_item->Cost());
}

void CItemManager::update			()
{
	START_PROFILE("Memory Manager/items::update")

#ifdef DEBUG
	for (auto &it: m_objects)
		VERIFY3(m_object->movement().restrictions().accessible(it->ai_location().level_vertex_id()), *m_object->cName(), *it->cName());
#endif // DEBUG

	inherited::update();
	VERIFY3(!selected() || m_object->movement().restrictions().accessible(selected()->ai_location().level_vertex_id()), *m_object->cName(), selected() ? *selected()->cName() : "<no selected item>");

	STOP_PROFILE
}

void CItemManager::remove_links(CObject *object)
{
	// since we use no members in CGameObject during search,
	// we just use the pinter itself, we can just statically cast object
	auto obj = std::find(m_objects.begin(),m_objects.end(), (CGameObject*)object);
	if (obj != m_objects.end())
		m_objects.erase(obj);

	if (m_selected && (m_selected->ID() == object->ID()))
		m_selected = 0;
}

void CItemManager::on_restrictions_change()
{
	if (m_selected)
	{

		if (!m_object->movement().restrictions().accessible(m_selected->ai_location().level_vertex_id())) {
			m_selected = 0;
			return;
		}
		if (m_object->movement().restrictions().accessible(m_selected->Position()))
			return;

		m_selected = 0;
	}
}
