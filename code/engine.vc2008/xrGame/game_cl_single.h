#pragma once
#include "game_cl_base.h"
#include "../xrScripts/export/script_export_space.h"

class game_cl_Single :public game_cl_GameState
{
	typedef game_cl_GameState inherited;
public :
										game_cl_Single			() = default;
	virtual		CUIGameCustom*			createGameUI			();
	virtual		bool					IsServerControlHits		()	{return true;};

	virtual		u64						GetStartGameTime		();
	virtual		u64						GetGameTime				();	
	virtual		float					GetGameTimeFactor		();	
	virtual		void					SetGameTimeFactor		(const float fTimeFactor);

	virtual		u64						GetEnvironmentGameTime		();
	virtual		float					GetEnvironmentGameTimeFactor();
	virtual		void					SetEnvironmentGameTimeFactor(const float fTimeFactor);

	void								OnDifficultyChanged		();
};

// game difficulty
enum ESingleGameDifficulty{
	egdNovice			= 0,
	egdStalker			= 1,
	egdVeteran			= 2,
	egdMaster			= 3,
	egdCount,
	egd_force_u32		= u32(-1)
};

extern ESingleGameDifficulty g_SingleGameDifficulty;
xr_token		difficulty_type_token	[ ];

typedef enum_exporter<ESingleGameDifficulty> CScriptGameDifficulty;
add_to_type_list(CScriptGameDifficulty)
#undef script_type_list
#define script_type_list save_type_list(CScriptGameDifficulty)




