
// helper functions --------------------------------------------

bool valid_saved_game_name(LPCSTR file_name)
{
	LPCSTR		I = file_name;
	LPCSTR		E = file_name + xr_strlen(file_name);
	for ( ; I != E; ++I) {
		if (!strchr("/\\:*?\"<>|^()[]%",*I))
			continue;

		return	(false);
	};

	return		(true);
}

void get_files_list( xr_vector<shared_str>& files, LPCSTR dir, LPCSTR file_ext )
{
	VERIFY( dir && file_ext );
	files.clear();

	FS_Path* P = FS.get_path( dir );
	P->m_Flags.set( FS_Path::flNeedRescan, TRUE );
	FS.m_Flags.set( CLocatorAPI::flNeedCheck, TRUE );
	FS.rescan_pathes();

	string256 fext;
	xr_strconcat( fext, "*", file_ext );

	FS_FileSet  files_set;
	FS.file_list( files_set, dir, FS_ListFiles, fext );
	u32 len_str_ext = xr_strlen( file_ext );

	for (const FS_File& elem : files_set)
	{
		LPCSTR fn_ext = elem.name.c_str();
		VERIFY(xr_strlen(fn_ext) > len_str_ext);
		string_path fn;
		strncpy_s(fn, sizeof(fn), fn_ext, xr_strlen(fn_ext) - len_str_ext);
		files.emplace_back(fn);
	}

	FS.m_Flags.set( CLocatorAPI::flNeedCheck, FALSE );
}

class CCC_ALifeSave : public IConsole_Command
{
public:
	CCC_ALifeSave(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {

		if (!g_actor || !Actor()->g_Alive())
		{
			Msg("cannot make saved game because actor is dead :(");
			return;
		}

		if (Actor()->HasInfo("cant_game_save_now"))
		{
			SDrawStaticStruct* _s = GameUI()->AddCustomStatic("game_saved", true);
			_s->wnd()->TextItemControl()->SetText(CStringTable().translate("st_cant_game_save_now").c_str());
			return;
		}

        string_path	GameSaveName = { 0 };
        string_path screenshotForSavePath = { 0 };

        xr_strcpy(GameSaveName, args);

#ifdef DEBUG
		CTimer timer;
		timer.Start();
#endif

		if (!xr_strlen(GameSaveName)) 
		{
            xr_sprintf(GameSaveName, "%s - quicksave", Core.UserName);
			NET_Packet net_packet;
			net_packet.w_stringZ(GameSaveName);
			net_packet.w_u8(0);

            if (ai().get_alife())
                Level().Server->game->alife().save(net_packet);
		}
		else 
		{
			if (!valid_saved_game_name(GameSaveName))
			{
				Msg("! Save failed: invalid file name - %s", GameSaveName);
				return;
			}

			NET_Packet net_packet;
			net_packet.w_stringZ(GameSaveName);
			net_packet.w_u8(1);
            if (ai().get_alife())
                Level().Server->game->alife().save(net_packet);
			//Level().Send(net_packet);
		}
#ifdef DEBUG
		Msg("Game save overhead  : %f milliseconds", timer.GetElapsed_sec()*1000.f);
#endif
		SDrawStaticStruct* _s = GameUI()->AddCustomStatic("game_saved", true);
		string256 save_name;
		xr_strconcat(save_name, CStringTable().translate("st_game_saved").c_str(), ": ", GameSaveName);
		_s->wnd()->TextItemControl()->SetText(save_name);

		xr_strcat(GameSaveName, ".dds");
		FS.update_path(screenshotForSavePath, "$game_saves$", GameSaveName);

#ifdef DEBUG
		timer.Start();
#endif
		MainMenu()->Screenshot(IRender_interface::SM_FOR_GAMESAVE, screenshotForSavePath);

#ifdef DEBUG
		Msg("Screenshot overhead : %f milliseconds", timer.GetElapsed_sec() * 1000.f);
#endif

	}

	virtual void fill_tips(vecTips& tips, u32 mode)
	{
		get_files_list(tips, "$game_saves$", SAVE_EXTENSION);
	}

};

class CCC_ALifeLoadFrom : public IConsole_Command 
{
public:
	CCC_ALifeLoadFrom(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute( LPCSTR args )
	{
		string_path				saved_game;
        xr_strcpy(saved_game, args);

		if (!ai().get_alife()) {
			Log						("! ALife simulator has not been started yet");
			return;
		}

		if (!xr_strlen(saved_game)) {
            xr_sprintf(saved_game, "%s - quicksave", Core.UserName);
		}

		if (!CSavedGameWrapper::saved_game_exist(saved_game)) {
			Msg						("! Cannot find saved game %s",saved_game);
			return;
		}

		if (!CSavedGameWrapper::valid_saved_game(saved_game)) {
			Msg						("! Cannot load saved game %s, version mismatch or saved game is corrupted",saved_game);
			return;
		}

		if ( !valid_saved_game_name(saved_game) )
		{
			Msg						("! Cannot load saved game %s, invalid file name",saved_game);
			return;
		}

		if(MainMenu()->IsActive())
			MainMenu()->Activate(false);

		if (Device.Paused())
			Device.Pause			(FALSE, TRUE, TRUE, "CCC_ALifeLoadFrom");

		NET_Packet					net_packet;
		net_packet.w_begin			(M_LOAD_GAME);
		net_packet.w_stringZ		(saved_game);
		Level().Send				(net_packet);
	}
	
	virtual void fill_tips			(vecTips& tips, u32 mode)
	{
		get_files_list				(tips, "$game_saves$", SAVE_EXTENSION);
	}

};


class CCC_ALifeTimeFactor : public IConsole_Command
{
public:
	CCC_ALifeTimeFactor(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) 
	{
		if (!g_pGameLevel)
			return;

		float id1 = 0.0f;
		sscanf(args, "%f", &id1);
		if (id1 < EPS_L)
			Msg("Invalid time factor! (%.4f)", id1);
		else
			Level().SetGameTimeFactor(id1);
	}
	
	virtual void	Save(IWriter *F) {};
	virtual void	Status(TStatus& S)
	{
		if (g_pGameLevel)
		{
			float v = Level().GetGameTimeFactor();
			xr_sprintf(S, sizeof(S), "%3.5f", v);
			while (xr_strlen(S) && ('0' == S[xr_strlen(S) - 1]))
				S[xr_strlen(S) - 1] = 0;
		}
	}
	virtual void	Info(TInfo& I)
	{	
		if (g_pGameLevel)
		{
			float v = Level().GetGameTimeFactor();
			xr_sprintf(I, sizeof(I), " value = %3.5f", v);
		}
	}
	virtual void	fill_tips(vecTips& tips, u32 mode)
	{
		if (g_pGameLevel)
		{
			float v = Level().GetGameTimeFactor();

			TStatus str;
			xr_sprintf(str, sizeof(str), "%3.5f  (current)  [0.0,1000.0]", v);
			tips.emplace_back(str);
			IConsole_Command::fill_tips(tips, mode);
		}
	}
};

class CCC_ALifeSwitchDistance : public IConsole_Command 
{
public:
	CCC_ALifeSwitchDistance(LPCSTR N) : IConsole_Command(N) { };

	virtual void Execute(LPCSTR args)
	{
		if (ai().get_alife())
		{
			float id1 = 0.0f;
			sscanf(args, "%f", &id1);
			if (id1 < 2.0f)
			{
				Msg("Invalid online distance! (%.4f)", id1);
			}
			else
			{
				Level().Server->game->alife().set_switch_distance(id1);
			}
		}
	}
};

class CCC_ALifeProcessTime : public IConsole_Command
{
public:
	CCC_ALifeProcessTime(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) 
	{
		if (ai().get_alife())
		{
			int id1 = 0;
			sscanf(args ,"%d",&id1);
			if (id1 < 1)
				Msg("Invalid process time! (%d)",id1);
			else
				Level().Server->game->alife().set_process_time(id1);
		}
	}

};

class CCC_ALifeObjectsPerUpdate : public IConsole_Command
{
public:
	CCC_ALifeObjectsPerUpdate(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) 
	{
		if (ai().get_alife())
		{
			int id1 = 0;
			sscanf(args ,"%d",&id1);
			Level().Server->game->alife().objects_per_update(id1);
		}
	}
};

class CCC_ALifeSwitchFactor : public IConsole_Command
{
public:
	CCC_ALifeSwitchFactor(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) 
	{
		if (ai().get_alife())
		{
			float id1 = 0;
			sscanf(args ,"%f",&id1);
			clamp(id1,.1f,1.f);
			Level().Server->game->alife().set_switch_factor(id1);
		}
	}
};
