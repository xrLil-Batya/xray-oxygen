﻿#pragma once

// refs
class ENGINE_API CGameFont;
class ILoadingScreen;

// definition
class ENGINE_API CApplication	:
	public pureFrame,
	public IEventReceiver
{

public:
	// levels
	struct					sLevelInfo
	{
		char*				folder;
		char*				name;
	};

private:
	ILoadingScreen * loadingScreen;

	int		max_load_stage;

	int						load_stage;

	u32						dwLoadReference;
private:
	EVENT					eQuit;
	EVENT					eStart;
	EVENT					eStartLoad;
	EVENT					eDisconnect;
	EVENT					eConsole;

	void					Level_Append		(LPCSTR lname);
public:
	CGameFont*				pFontSystem;

	// Levels
	xr_vector<sLevelInfo>	Levels;
	u32						Level_Current;
	void					Level_Scan			();
	int						Level_ID			(LPCSTR name, LPCSTR ver, bool bSet);
	void					Level_Set			(u32 ID);
	void					LoadAllArchives		();
    static CInifile*				GetArchiveHeader	(LPCSTR name, LPCSTR ver);

	// Loading
	void					LoadBegin			();
	void					LoadEnd				();
	void					LoadTitleInt		(LPCSTR str1, LPCSTR str2, LPCSTR str3);
	void					LoadStage			();
	void					LoadSwitch			();
	void					LoadDraw			();
	void					LoadForceFinish		();

	void					SetLoadStageTitle		(const char* ls_title);

	virtual	void			OnEvent				(EVENT E, u64 P1, u64 P2);

	// Other
							CApplication		();
	virtual					~CApplication		();

	virtual void	_BCL	OnFrame				();
			void			load_draw_internal	();

			void SetLoadingScreen				(ILoadingScreen* newScreen);
			void DestroyLoadingScreen			();
};

extern ENGINE_API	CApplication*	pApp;
extern ENGINE_API   BOOL g_appLoaded;
