#include "stdafx.h"
#pragma hdrstop


#include "gametype_chooser.h"
#include "xrServer_Objects_Abstract.h"
//old
enum ERPGameType{		// [0..255]
	rpgtGameAny							= u8(0),
	rpgtGameDeathmatch,
	rpgtGameTeamDeathmatch,
	rpgtGameArtefactHunt,
	rpgtGameCaptureTheArtefact,
	rpgtGameCount,
};

xr_token rpoint_game_type[]={
	{ "Any game",			rpgtGameAny					},
	{ "Deathmatch",			rpgtGameDeathmatch			},
	{ "TeamDeathmatch",		rpgtGameTeamDeathmatch		},
	{ "ArtefactHunt",		rpgtGameArtefactHunt		},
	{ "CaptureTheArtefact",	rpgtGameCaptureTheArtefact	},
	{ 0,					0	}
};


#ifdef _EDITOR
bool GameTypeChooser::LoadStream(IReader& F)
{
    m_GameType.assign	(F.r_u16());

    return true;
}

bool GameTypeChooser::LoadLTX(CInifile& ini, LPCSTR sect_name, bool bOldFormat)
{
    if(bOldFormat/*version==0x0014*/)
    {
        u8 tmp 					= ini.r_u8	(sect_name, "game_type");
        m_GameType.zero		();
        if(tmp == rpgtGameAny)
			m_GameType.one();
    }
	else m_GameType.assign(ini.r_u16	(sect_name, "game_type"));
    return true;
}

void GameTypeChooser::SaveStream(IWriter& F)
{
   F.w_u16 	(m_GameType.get());
}

void GameTypeChooser::SaveLTX(CInifile& ini, LPCSTR sect_name)
{
  ini.w_u16(sect_name, "game_type", m_GameType.get());
}
#endif

#ifndef XRGAME_EXPORTS
void  GameTypeChooser::FillProp(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateGameType		(items, PrepareKey(pref, "Game Type"), this);
 }
#endif // #ifndef XRGAME_EXPORTS