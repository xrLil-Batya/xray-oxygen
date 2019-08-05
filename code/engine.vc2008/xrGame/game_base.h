#pragma once
#include "../xrScripts/export/script_export_space.h"
#include "../xrServerEntities/alife_space.h"
#include "../xrServerEntities/gametype_chooser.h"
#pragma pack(push,1)
class NET_Packet;

#pragma pack(pop)

class game_GameState : public DLL_Pure
{
protected:
	EGameIDs						m_type;
	u16								m_phase;
	u32								m_start_time;

	u32								m_round_start_time;
	string64						m_round_start_time_str;
protected:
	virtual		void				switch_Phase			(u32 new_phase);

public:
									game_GameState			();
	virtual							~game_GameState			()								{}

	virtual		void				net_import_state(NET_Packet& P);
	// update GameTime only for remote clients
	virtual		void				net_import_GameTime(NET_Packet& P); 

	IC			EGameIDs const&		Type					() const						{return m_type;};
				u16					Phase					() const						{return m_phase;};
				u32					StartTime				() const						{return m_start_time;};
	virtual		void				Create					(shared_str& options)			{};
//for scripting enhancement
	static		CLASS_ID			getCLASS_ID				(LPCSTR game_type_name, bool bServer);

//moved from game_sv_base (time routines)
private:
	// scripts
	u64								m_qwStartProcessorTime;
	u64								m_qwStartGameTime;
	float							m_fTimeFactor;
	//-------------------------------------------------------
	u64								m_qwEStartProcessorTime;
	u64								m_qwEStartGameTime;
	float							m_fETimeFactor;
	//-------------------------------------------------------
public:

	virtual		u64					GetStartGameTime		();
	virtual		u64					GetGameTime				();	
	virtual		float				GetGameTimeFactor		();	
				void				SetGameTimeFactor		(u64 GameTime, const float fTimeFactor);
	virtual		void				SetGameTimeFactor		(const float fTimeFactor);
	

	virtual		u64					GetEnvironmentGameTime	();
	virtual		float				GetEnvironmentGameTimeFactor		();
	virtual		void				SetEnvironmentGameTimeFactor		(u64 GameTime, const float fTimeFactor);
	virtual		void				SetEnvironmentGameTimeFactor		(const float fTimeFactor);

	virtual		void				SendPickUpEvent						(u16 ID_who, u16 ID_what);
};

enum EGamePhases
{
	GAME_PHASE_NONE = 0,
	GAME_PHASE_INPROGRESS,
	GAME_PHASE_FORCEDWORD = u32(-1)
};

// game difficulty
enum ESingleGameDifficulty
{
	egdNovice = 0,
	egdStalker = 1,
	egdVeteran = 2,
	egdMaster = 3,
	egdCount,
	egd_force_u32 = u32(-1)
};
extern GAME_API ESingleGameDifficulty g_SingleGameDifficulty;

xr_token		difficulty_type_token[];
xr_token		language_type_token[];

using CScriptGameDifficulty = enum_exporter<ESingleGameDifficulty>;
add_to_type_list(CScriptGameDifficulty)
#undef script_type_list
#define script_type_list save_type_list(CScriptGameDifficulty)