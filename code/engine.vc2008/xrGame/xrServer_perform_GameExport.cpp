#include "stdafx.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "Level.h"

void xrServer::Perform_game_export	()
{
    xrClientData* CL = (xrClientData*)SV_Client;
    if (!CL->net_Accepted)
        return;

    NET_Packet P;
    P.w_begin(M_SV_CONFIG_GAME);
    game->net_Export_State(P, SV_Client->ID);
	Level().OnMessage(P.B.data, (u32)P.B.count);

	game->sv_force_sync	= FALSE;
}