#include "stdafx.h"
#include "xrserver.h"
#include "xrserver_objects.h"

void xrServer::Process_event_activate(NET_Packet& P, const ClientID sender, const u32 time, const u16 id_parent, const u16 id_entity, bool send_message)
{
	// Parse message
	CSE_Abstract*		e_parent = game->get_entity_from_eid(id_parent);
	CSE_Abstract*		e_entity = game->get_entity_from_eid(id_entity);

	R_ASSERT2(e_parent, make_string("parent not found. id_parent=%d id_entity=%d frame=%d", id_parent, id_entity, Device.dwFrame).c_str());
	R_ASSERT2(e_entity, make_string("entity not found. id_parent=%d id_entity=%d frame=%d", id_parent, id_entity, Device.dwFrame).c_str());

	xr_delete(e_parent);

	if (0xffff != e_entity->ID_Parent && send_message)
	{
		// Signal to everyone (including sender)
		SendBroadcast(BroadcastCID, P);
	}
}