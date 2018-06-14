#include "stdafx.h"
#pragma hdrstop


#include "gametype_chooser.h"
#include "xrServer_Objects_Abstract.h"
//old
enum ERPGameType{		// [0..255]
	rpgtGameAny = u8(0),
	rpgtGameCount,
};

xr_token rpoint_game_type[]={
	{ "Any game",			rpgtGameAny					},
	{ 0,					0	}
};

#ifndef XRGAME_EXPORTS
void  GameTypeChooser::FillProp(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateGameType(items, PrepareKey(pref, "Game Type"), this);
 }
#endif // #ifndef XRGAME_EXPORTS