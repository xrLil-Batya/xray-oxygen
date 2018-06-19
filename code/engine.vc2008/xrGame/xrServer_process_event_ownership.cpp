#include "stdafx.h"
#include "xrserver.h"
#include "xrserver_objects.h"
#include "xrserver_objects_alife_monsters.h"

void xrServer::Process_event_ownership(NET_Packet& P, u16 ID)
{
	u16					id_parent = ID, id_entity;
	P.r_u16				(id_entity);
	CSE_Abstract*		e_parent	= game->get_entity_from_eid	(id_parent);
	CSE_Abstract*		e_entity	= game->get_entity_from_eid	(id_entity);
	
	if ( !e_parent ) {
		Msg( "! ERROR on ownership: parent not found. parent_id = [%d], entity_id = [%d], frame = [%d].", id_parent, id_entity, Device.dwFrame );
		return;
	}
	if ( !e_entity ) {
		return;
	}
	
	if (!is_object_valid_on_svclient(id_parent))
	{
		Msg( "! ERROR on ownership: parent object is not valid on sv client. parent_id = [%d], entity_id = [%d], frame = [%d]", id_parent, id_entity, Device.dwFrame );
		return;
	}

	if (!is_object_valid_on_svclient(id_entity))
	{
		Msg( "! ERROR on ownership: entity object is not valid on sv client. parent_id = [%d], entity_id = [%d], frame = [%d]", id_parent, id_entity, Device.dwFrame );
		return;
	}

	if (0xffff != e_entity->ID_Parent)
        return;

	// Game allows ownership of entity
	game->OnTouch(id_parent, id_entity);
	
	{
		// Rebuild parentness
		e_entity->ID_Parent			= id_parent;
		e_parent->children.push_back(id_entity);

		// Signal to everyone (including sender)
		SendBroadcast(BroadcastCID,P);
	}
}
