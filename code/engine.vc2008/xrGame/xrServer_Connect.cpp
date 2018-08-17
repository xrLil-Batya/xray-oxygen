#include "stdafx.h"
#include "xrserver.h"
#include "xrMessages.h"
#include "MainMenu.h"
#include "../xrEngine/x_ray.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

xrServer::EConnect xrServer::Connect(shared_str &session_name)
{
	// Parse options and create game
	if (!strchr(*session_name,'/'))
		return				ErrConnect;

	string1024				options;
	R_ASSERT2(xr_strlen(session_name) <= sizeof(options), "session_name too BIIIGGG!!!");
	xr_strcpy					(options,strchr(*session_name,'/')+1);
	
	// Parse game type
	string1024				type;
	R_ASSERT2(xr_strlen(options) <= sizeof(type), "session_name too BIIIGGG!!!");
	xr_strcpy					(type,options);
	if (strchr(type,'/'))	*strchr(type,'/') = 0;
	game					= nullptr;

	CLASS_ID clsid			= game_GameState::getCLASS_ID(type,true);
	game					= smart_cast<game_sv_GameState*> (NEW_INSTANCE(clsid));
	
	// Options
	if (nullptr==game)			return ErrConnect;
	
	game->Create			(session_name);

    connect_options = session_name;
	return ErrNoError;
}
