#include "stdafx.h"
#include "xr_ioconsole.h"
#include "xr_input.h"
#include "xr_ioc_cmd.h"
#include "xr_level_controller.h"
#include "string_table.h"

_binding g_key_bindings	[bindings_count]; 

_action  actions[] = {
	{ "left",				kLEFT				},
	{ "right",				kRIGHT				},
	{ "up",					kUP					},
	{ "down",				kDOWN				},
	{ "jump",				kJUMP				},
	{ "crouch",				kCROUCH				},
	{ "accel",				kACCEL				},
	{ "sprint_toggle",  	kSPRINT_TOGGLE		},

	{ "forward",			kFWD				},
	{ "back",				kBACK				},
	{ "lstrafe",			kL_STRAFE			},
	{ "rstrafe",			kR_STRAFE			},

	{ "llookout",			kL_LOOKOUT			},
	{ "rlookout",			kR_LOOKOUT			},

	{ "cam_1",				kCAM_1				},
	{ "cam_2",				kCAM_2				},
	{ "cam_3",				kCAM_3				},
	{ "cam_zoom_in",		kCAM_ZOOM_IN		},
	{ "cam_zoom_out",		kCAM_ZOOM_OUT		},

	{ "torch",				kTORCH				},
	{ "night_vision",		kNIGHT_VISION		},
	{ "show_detector",		kDETECTOR			},

	{ "turn_engine",		kTURN_ENGINE		},
	{ "switch_horn",		kSWITCH_HORN		},

	{ "wpn_1",				kWPN_1				},
	{ "wpn_2",				kWPN_2				},
	{ "wpn_3",				kWPN_3				},
	{ "wpn_4",				kWPN_4				},
	{ "wpn_5",				kWPN_5				},
	{ "wpn_6",				kWPN_6				},
	{ "wpn_next",			kWPN_NEXT			},
	{ "wpn_fire",			kWPN_FIRE			},
	{ "wpn_zoom",			kWPN_ZOOM			},
	{ "wpn_reload",			kWPN_RELOAD			},
	{ "wpn_func",			kWPN_FUNC			},
	{ "wpn_firemode_prev",	kWPN_FIREMODE_PREV	},
	{ "wpn_firemode_next",	kWPN_FIREMODE_NEXT	},

	{ "pause",				kPAUSE				},
	{ "drop",				kDROP				},
	{ "use",				kUSE				},
	{ "scores",				kSCORES				},
	{ "screenshot",			kSCREENSHOT			},
	{ "quit",				kQUIT				},
	{ "console",			kCONSOLE			},
	{ "inventory",			kINVENTORY			},
	{ "active_jobs",		kACTIVE_JOBS		},
	{ "next_slot",			kNEXT_SLOT			},
	{ "prev_slot",			kPREV_SLOT			},

	{ "quick_use_1",		kQUICK_USE_1		},
	{ "quick_use_2",		kQUICK_USE_2		},
	{ "quick_use_3",		kQUICK_USE_3		},
	{ "quick_use_4",		kQUICK_USE_4		},

	{ "quick_save",			kQUICK_SAVE			},
	{ "quick_load",			kQUICK_LOAD			},

	{ "dev_noclip",			kDEV_NOCLIP			},
	{ "dev_action1",		kDEV_ACTION1		},
	{ "dev_action2",		kDEV_ACTION2		},
	{ "dev_action3",		kDEV_ACTION3		},
	{ "dev_action4",		kDEV_ACTION4		},

	{ nullptr, 				kLASTACTION			}
};

_keyboard keyboards[] = {
	{ "kESCAPE",	 	VK_ESCAPE		},	{ "k1",				VK_1			},
	{ "k2",				VK_2			},	{ "k3",				VK_3			},
	{ "k4",				VK_4			},	{ "k5",				VK_5			},
	{ "k6",				VK_6			},	{ "k7",				VK_7			},
	{ "k8",				VK_8			},	{ "k9",				VK_9			},
	{ "k0",				VK_0			},	{ "kMINUS",			VK_OEM_MINUS	},
	{ "kEQUALS",		VK_OEM_PLUS		},	{ "kBACK",			VK_BACK			},
	{ "kTAB",			VK_TAB 			},	{ "kQ",				VK_Q			},
	{ "kW",				VK_W			},	{ "kE",				VK_E			},
	{ "kR",				VK_R			},	{ "kT",				VK_T			},
	{ "kY",				VK_Y			},	{ "kU",				VK_U			},
	{ "kI",				VK_I			},	{ "kO",				VK_O			},
	{ "kP",				VK_P			},	{ "kLBRACKET",		VK_OEM_4		},
	{ "kRBRACKET",		VK_OEM_6		},	{ "kRETURN",		VK_RETURN		},
	{ "kLCONTROL",		VK_LCONTROL		},	{ "kA",				VK_A			},
	{ "kS",				VK_S			},	{ "kD",				VK_D			},
	{ "kF",				VK_F			},	{ "kG",				VK_G			},
	{ "kH",				VK_H			},	{ "kJ",				VK_J			},
	{ "kK",				VK_K			},	{ "kL",				VK_L			},
	{ "kSEMICOLON",		VK_OEM_1		},	{ "kAPOSTROPHE",	VK_OEM_3		},
	{ "kGRAVE",			VK_OEM_3		},	{ "kLSHIFT",	 	VK_SHIFT		},
	{ "kBACKSLASH",		VK_OEM_5		},	{ "kZ",				VK_Z			},
	{ "kX",				VK_X			},	{ "kC",				VK_C			},
	{ "kV",				VK_V			},	{ "kB",				VK_B			},
	{ "kN",				VK_N			},	{ "kM",				VK_M			},
	{ "kCOMMA",			VK_OEM_COMMA	},	{ "kPERIOD",		VK_OEM_PERIOD	},
	{ "kSLASH",			VK_OEM_2		},	{ "kRSHIFT",		VK_RSHIFT		},
	{ "kMULTIPLY",		VK_MULTIPLY		},	{ "kLMENU",			VK_LMENU		},
	{ "kSPACE",			VK_SPACE		},	{ "kCAPITAL",		VK_CAPITAL		},
	{ "kF1",			VK_F1			},	{ "kF2",			VK_F2			},
	{ "kF3",			VK_F3			},	{ "kF4",			VK_F4			},
	{ "kF5",			VK_F5			},	{ "kF6",			VK_F6			},
	{ "kF7",			VK_F7			},	{ "kF8",			VK_F8			},
	{ "kF9",			VK_F9			},	{ "kF10",			VK_F10			},
	{ "kNUMLOCK",		VK_NUMLOCK		},	{ "kSCROLL",		VK_SCROLL		},
	{ "kNUMPAD7",		VK_NUMPAD7		},	{ "kNUMPAD8",		VK_NUMPAD8		},
	{ "kNUMPAD9",		VK_NUMPAD9		},	{ "kSUBTRACT",		VK_SUBTRACT	},
	{ "kNUMPAD4",		VK_NUMPAD4		},	{ "kNUMPAD5",		VK_NUMPAD5		},
	{ "kNUMPAD6",		VK_NUMPAD6		},	{ "kADD",			VK_ADD			},
	{ "kNUMPAD1",		VK_NUMPAD1		},	{ "kNUMPAD2",		VK_NUMPAD2		},
	{ "kNUMPAD3",		VK_NUMPAD3		},	{ "kNUMPAD0",		VK_NUMPAD0		},
	{ "kDECIMAL",		VK_DECIMAL		},	{ "kF11",			VK_F11			},
	{ "kF12",			VK_F12			},	{ "kF13",			VK_F13			},
	{ "kF14",			VK_F14			},	{ "kF15",			VK_F15			},
	{ "kKANA",			VK_KANA			},	{ "kCONVERT",		VK_CONVERT		},
	{ "kNOCONVERT",		VK_NONCONVERT	},
	{ "kRCONTROL",		VK_RCONTROL		},	
	{ "kDIVIDE",		VK_DIVIDE		},	
	{ "kRMENU",			VK_RMENU		},	{ "kHOME",			VK_HOME			},
	{ "kUP",			VK_UP			},	{ "kPRIOR",			VK_PRIOR		},
	{ "kLEFT",			VK_LEFT			},	{ "kRIGHT",			VK_RIGHT		},
	{ "kEND",			VK_END			},	{ "kDOWN",			VK_DOWN			},
	{ "kNEXT",			VK_NEXT			},	{ "kINSERT",		VK_INSERT		},
	{ "kDELETE",		VK_DELETE		},	{ "kLWIN",			VK_LWIN			},
	{ "kRWIN",			VK_RWIN			},	{ "kAPPS",			VK_APPS			},
	{ "kPAUSE",			VK_PAUSE		},	{ "mouse1",			VK_LBUTTON		},
	{ "mouse2",			VK_RBUTTON		},	{ "mouse3",			VK_MBUTTON		},
	{ "mouse4",			VK_XBUTTON1		},	{ "mouse5",			VK_XBUTTON2		},
	{ "kGPD_A",			VK_GAMEPAD_A	},  { "kGPD_B",			VK_GAMEPAD_B	},
	{ "kGPD_X",			VK_GAMEPAD_X	},  { "kGPD_Y",			VK_GAMEPAD_Y	},
	{ "kGPD_TRG_L",		VK_GAMEPAD_LEFT_TRIGGER	},  { "kGPD_TRG_R",			VK_GAMEPAD_RIGHT_TRIGGER	},
	{ NULL, 			0				}
};

void initialize_bindings()
{
#ifdef DEBUG
	int i1 = 0;
	while (true)
	{
		_keyboard& _k1 = keyboards[i1];
		if (!_k1.key_name)	
			break;

		int i2 = i1;

		while (true)
		{
			_keyboard& _k2 = keyboards[i2];
			if (!_k2.key_name)	
				break;

			if (_k1.dik == _k2.dik && i1 != i2)
				Msg("%s==%s", _k1.key_name, _k2.key_name);

			++i2;
		}
		++i1;
	}
#endif

	for (int idx = 0; idx < bindings_count; ++idx)
		g_key_bindings[idx].m_action = &actions[idx];
}

void remap_keys()
{
	int idx = 0;
	string64 buff;
	while (keyboards[idx].key_name)
	{
		buff[0] = 0;
		_keyboard& kb = keyboards[idx];

		bool res = pInput->get_VK_name(kb.dik, buff, sizeof(buff));
		kb.key_local_name = res ? buff : kb.key_name;

		++idx;
	}
}

LPCSTR id_to_action_name(EGameActions _id)
{
	int idx = 0;
	while (actions[idx].action_name)
	{
		if (_id == actions[idx].id)
			return actions[idx].action_name;
		++idx;
	}

	Msg("! cant find corresponding [action_name] for id '%s'", _id);
	return nullptr;
}

EGameActions action_name_to_id(LPCSTR _name)
{
	_action* action = action_name_to_ptr(_name);

	return action ? action->id : kNOTBINDED;
}

_action* action_name_to_ptr(LPCSTR _name)
{
	int idx = 0;
	while (actions[idx].action_name)
	{
		if (!stricmp(_name, actions[idx].action_name))
			return &actions[idx];

		++idx;
	}

	Msg("! cant find corresponding [id] for '%s'", _name);
	return nullptr;
}

LPCSTR	VK_to_keyname(u8 _dik)
{
	_keyboard* kb = VK_to_ptr(_dik, true);

	return kb ? kb->key_name : nullptr;
}

_keyboard* VK_to_ptr(u8 _dik, bool bSafe)
{
	int idx = 0;
	while (keyboards[idx].key_name)
	{
		_keyboard&	kb = keyboards[idx];
		if (kb.dik == _dik)
			return &keyboards[idx];
		++idx;
	}

	if (!bSafe)
		Msg("! cant find corresponding [_keyboard] for dik");

	return nullptr;
}

u8 keyname_to_dik(LPCSTR _name)
{
	_keyboard* _kb = keyname_to_ptr(_name);
	return _kb->dik;
}

_keyboard*	keyname_to_ptr(LPCSTR _name)
{
	int idx = 0;
	while (keyboards[idx].key_name)
	{
		_keyboard&	kb = keyboards[idx];
		if (!stricmp(_name, kb.key_name))
			return &keyboards[idx];
		++idx;
	}

	Msg("! cant find corresponding [_keyboard*] for keyname %s", _name);
	return nullptr;
}

bool is_binded(EGameActions _action_id, u8 _dik)
{
	_binding* pbinding = &g_key_bindings[_action_id];
	if (pbinding->m_keyboard[0] && pbinding->m_keyboard[0]->dik == _dik)
		return true;

	if (pbinding->m_keyboard[1] && pbinding->m_keyboard[1]->dik == _dik)
		return true;

	return false;
}

int get_action_dik(EGameActions _action_id, int idx)
{
	_binding* pbinding = &g_key_bindings[_action_id];

	if (idx == -1)
	{
		if (pbinding->m_keyboard[0])
			return pbinding->m_keyboard[0]->dik;

		if (pbinding->m_keyboard[1])
			return pbinding->m_keyboard[1]->dik;
	}
	else
	{
		if (pbinding->m_keyboard[idx])
			return pbinding->m_keyboard[idx]->dik;
	}
	return 0;
}

EGameActions get_binded_action(u8 dik)
{
	for (_binding & g_key_binding : g_key_bindings)
	{
		_binding* binding = &g_key_binding;

		if (binding->m_keyboard[0] && binding->m_keyboard[0]->dik == dik)
			return binding->m_action->id;

		if (binding->m_keyboard[1] && binding->m_keyboard[1]->dik == dik)
			return binding->m_action->id;
	}

	return kNOTBINDED;
}

void GetActionAllBinding(LPCSTR _action, char* dst_buff, int dst_buff_sz)
{
	int action_id = action_name_to_id(_action);
	_binding* pbinding = &g_key_bindings[action_id];

	string128 prim;
	string128 sec;
	prim[0] = 0;
	sec[0] = 0;

	if (pbinding->m_keyboard[0])
	{
		try
		{
			xr_strcpy(prim, pbinding->m_keyboard[0]->key_local_name.c_str());
		}
		catch (...)
		{
			R_ASSERT_FORMAT(false, "can't action id: %d", action_id);
		}
	}

	if (pbinding->m_keyboard[1])
		xr_strcpy(sec, pbinding->m_keyboard[1]->key_local_name.c_str());
	if (!pbinding->m_keyboard[0] && !pbinding->m_keyboard[1])
		xr_sprintf(dst_buff, dst_buff_sz, "%s", CStringTable().translate("st_key_notbinded").c_str());
	else
		xr_sprintf(dst_buff, dst_buff_sz, "%s%s%s", prim[0] ? prim : "", (sec[0] && prim[0]) ? " , " : "", sec[0] ? sec : "");
}

ConsoleBindCmds	bindConsoleCmds;
bool bRemapped = false;

class CCC_Bind : public IConsole_Command
{
	int m_work_idx;

public:
	CCC_Bind(LPCSTR N, int idx) : IConsole_Command(N), m_work_idx(idx) {};
	virtual void Execute(LPCSTR args)
	{
		string256 action;
		string256 key;
		*action = 0;
		*key = 0;

		sscanf(args, "%s %s", action, key);
		if (!*action)
			return;

		if (!*key)
			return;

		if (!bRemapped) 
		{
			remap_keys();
			bRemapped = true;
		}

		if (!action_name_to_ptr(action))
			return;

		int action_id = action_name_to_id(action);
		if (action_id == kNOTBINDED)
			return;

		_keyboard*	pkeyboard = keyname_to_ptr(key);
		if (!pkeyboard)
			return;

		_binding*	curr_pbinding = &g_key_bindings[action_id];

		curr_pbinding->m_keyboard[m_work_idx] = pkeyboard;

		{
			for (_binding & g_key_binding : g_key_bindings)
			{
				_binding*	binding = &g_key_binding;
				if (binding == curr_pbinding)
					continue;

				if (binding->m_keyboard[0] == pkeyboard)
					binding->m_keyboard[0] = nullptr;

				if (binding->m_keyboard[1] == pkeyboard)
					binding->m_keyboard[1] = nullptr;
			}
		}

		CStringTable::ReparseKeyBindings();
	}

	virtual void Save(IWriter* F)
	{
		if (m_work_idx == 0)
			F->w_printf("default_controls\r\n");

		for (_binding & g_key_binding : g_key_bindings)
		{
			_binding* pbinding = &g_key_binding;
			if (pbinding->m_keyboard[m_work_idx])
			{
				F->w_printf("%s %s %s\r\n",
					cName,
					pbinding->m_action->action_name,
					pbinding->m_keyboard[m_work_idx]->key_name);
			}
		}
	}
};

class CCC_UnBind : public IConsole_Command
{
	int m_work_idx;

public:
	CCC_UnBind(LPCSTR N, int idx) : IConsole_Command(N), m_work_idx(idx)
	{
		bEmptyArgsHandled = true;
	};

	virtual void Execute(LPCSTR args)
	{
		int action_id = action_name_to_id(args);
		_binding*	pbinding = &g_key_bindings[action_id];
		pbinding->m_keyboard[m_work_idx] = nullptr;

		CStringTable::ReparseKeyBindings();
	}
};

class CCC_ListActions : public IConsole_Command
{
public:
	CCC_ListActions(LPCSTR N) : IConsole_Command(N)
	{
		bEmptyArgsHandled = true;
	};

	virtual void Execute(LPCSTR args) {
		Log("- --- Action list start ---");
		for (_binding & g_key_binding : g_key_bindings)
		{
			_binding* pbinding = &g_key_binding;
			Msg("-%s", pbinding->m_action->action_name);
		}

		Log("- --- Action list end   ---");
	}
};

class CCC_UnBindAll : public IConsole_Command
{
public:
	CCC_UnBindAll(LPCSTR N) : IConsole_Command(N)
	{
		bEmptyArgsHandled = true;
	};

	virtual void Execute(LPCSTR args)
	{
		for (_binding & g_key_binding : g_key_bindings)
		{
			_binding* pbinding = &g_key_binding;
			pbinding->m_keyboard[0] = nullptr;
			pbinding->m_keyboard[1] = nullptr;
		}

		bindConsoleCmds.clear();
	}
};

class CCC_DefControls : public CCC_UnBindAll
{
public:
	CCC_DefControls(LPCSTR N) : CCC_UnBindAll(N) {}

	virtual void Execute(LPCSTR args)
	{
		CCC_UnBindAll::Execute(args);
		string_path _cfg;
		string_path	cmd;
		FS.update_path(_cfg, "$game_config$", "default_controls.ltx");
		xr_strconcat(cmd, "cfg_load", " ", _cfg);
		Console->Execute(cmd);
	}
};

class CCC_BindList : public IConsole_Command
{
public:
	CCC_BindList(LPCSTR N) : IConsole_Command(N)
	{
		bEmptyArgsHandled = true;
	};

	virtual void Execute(LPCSTR args) {
		Log("- --- Bind list start ---");
		string512 buff;

		for (_binding & g_key_binding : g_key_bindings)
		{
			_binding* pbinding = &g_key_binding;
			xr_sprintf(buff, "[%s] primary is[%s] secondary is[%s]",
				pbinding->m_action->action_name,
				(pbinding->m_keyboard[0]) ? pbinding->m_keyboard[0]->key_local_name.c_str() : "NULL",
				(pbinding->m_keyboard[1]) ? pbinding->m_keyboard[1]->key_local_name.c_str() : "NULL");
			Log(buff);
		}
		Log("- --- Bind list end   ---");
	}
};

class CCC_BindConsoleCmd : public IConsole_Command
{
public:
	CCC_BindConsoleCmd(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args)
	{
		string512 console_command;
		string256 key;
		int cnt = _GetItemCount(args, ' ');
		_GetItems(args, 0, cnt - 1, console_command, ' ');
		_GetItem(args, cnt - 1, key, ' ');

		u8 dik = keyname_to_dik(key);
		bindConsoleCmds.bind(dik, console_command);
	}

	virtual void Save(IWriter* F)
	{
		bindConsoleCmds.save(F);
	}
};


class CCC_UnBindConsoleCmd : public IConsole_Command
{
public:
	CCC_UnBindConsoleCmd(LPCSTR N) : IConsole_Command(N)
	{
		bEmptyArgsHandled = false;
	};

	virtual void Execute(LPCSTR args)
	{
		u8 _dik = keyname_to_dik(args);
		bindConsoleCmds.unbind(_dik);
	}
};

void ConsoleBindCmds::bind(u8 dik, LPCSTR N)
{
	_conCmd& c = m_bindConsoleCmds[dik];
	c.cmd = N;
}

void ConsoleBindCmds::unbind(u8 dik)
{
	xr_map<u8, _conCmd>::iterator it = m_bindConsoleCmds.find(dik);
	if (it == m_bindConsoleCmds.end())
		return;

	m_bindConsoleCmds.erase(it);
}

void ConsoleBindCmds::clear()
{
	m_bindConsoleCmds.clear();
}

bool ConsoleBindCmds::execute(u8 dik)
{
	xr_map<u8, _conCmd>::iterator it = m_bindConsoleCmds.find(dik);
	if (it == m_bindConsoleCmds.end())
		return false;

	Console->Execute(it->second.cmd.c_str());
	return true;
}

void ConsoleBindCmds::save(IWriter* F)
{
	xr_map<u8, _conCmd>::iterator it = m_bindConsoleCmds.begin();

	for (; it != m_bindConsoleCmds.end(); ++it)
	{
		LPCSTR keyname = VK_to_keyname(it->first);
		F->w_printf("bind_console %s %s\n", *it->second.cmd, keyname);
	}
}


void CCC_RegisterInput()
{
	initialize_bindings									();
	CMD2(CCC_Bind,				"bind",					0);
	CMD2(CCC_Bind,				"bind_sec",				1);
	CMD2(CCC_UnBind,			"unbind",				0);
	CMD2(CCC_UnBind,			"unbind_sec",			1);
	CMD1(CCC_UnBindAll,			"unbindall"				);
	CMD1(CCC_DefControls,		"default_controls"		);
	CMD1(CCC_ListActions,		"list_actions"			);

	CMD1(CCC_BindList,			"bind_list"				);
	CMD1(CCC_BindConsoleCmd,	"bind_console"			);
	CMD1(CCC_UnBindConsoleCmd,	"unbind_console"		);
};

#define DECLARE_KEY_ENTRY(keyName) { keyName, #keyName},

const xr_map<u8, xr_string> KeyNamesTable =
{
	DECLARE_KEY_ENTRY(VK_LBUTTON)
	DECLARE_KEY_ENTRY(VK_RBUTTON)
	DECLARE_KEY_ENTRY(VK_CANCEL)
	DECLARE_KEY_ENTRY(VK_MBUTTON)
	DECLARE_KEY_ENTRY(VK_XBUTTON1)
	DECLARE_KEY_ENTRY(VK_XBUTTON2)
	DECLARE_KEY_ENTRY(VK_BACK)
	DECLARE_KEY_ENTRY(VK_TAB)
	DECLARE_KEY_ENTRY(VK_CLEAR)
	DECLARE_KEY_ENTRY(VK_RETURN)
	DECLARE_KEY_ENTRY(VK_SHIFT)
	DECLARE_KEY_ENTRY(VK_CONTROL)
	DECLARE_KEY_ENTRY(VK_MENU)
	DECLARE_KEY_ENTRY(VK_PAUSE)
	DECLARE_KEY_ENTRY(VK_CAPITAL)
	DECLARE_KEY_ENTRY(VK_KANA)
	DECLARE_KEY_ENTRY(VK_HANGEUL)
	DECLARE_KEY_ENTRY(VK_HANGUL)
	DECLARE_KEY_ENTRY(VK_JUNJA)
	DECLARE_KEY_ENTRY(VK_FINAL)
	DECLARE_KEY_ENTRY(VK_HANJA)
	DECLARE_KEY_ENTRY(VK_KANJI)
	DECLARE_KEY_ENTRY(VK_ESCAPE)
	DECLARE_KEY_ENTRY(VK_CONVERT)
	DECLARE_KEY_ENTRY(VK_NONCONVERT)
	DECLARE_KEY_ENTRY(VK_ACCEPT)
	DECLARE_KEY_ENTRY(VK_MODECHANGE)
	DECLARE_KEY_ENTRY(VK_SPACE)
	DECLARE_KEY_ENTRY(VK_PRIOR)
	DECLARE_KEY_ENTRY(VK_NEXT)
	DECLARE_KEY_ENTRY(VK_END)
	DECLARE_KEY_ENTRY(VK_HOME)
	DECLARE_KEY_ENTRY(VK_LEFT)
	DECLARE_KEY_ENTRY(VK_UP)
	DECLARE_KEY_ENTRY(VK_RIGHT)
	DECLARE_KEY_ENTRY(VK_DOWN)
	DECLARE_KEY_ENTRY(VK_SELECT)
	DECLARE_KEY_ENTRY(VK_PRINT)
	DECLARE_KEY_ENTRY(VK_EXECUTE)
	DECLARE_KEY_ENTRY(VK_SNAPSHOT)
	DECLARE_KEY_ENTRY(VK_INSERT)
	DECLARE_KEY_ENTRY(VK_DELETE)
	DECLARE_KEY_ENTRY(VK_HELP)
	DECLARE_KEY_ENTRY(VK_0)
	DECLARE_KEY_ENTRY(VK_1)
	DECLARE_KEY_ENTRY(VK_2)
	DECLARE_KEY_ENTRY(VK_3)
	DECLARE_KEY_ENTRY(VK_4)
	DECLARE_KEY_ENTRY(VK_5)
	DECLARE_KEY_ENTRY(VK_6)
	DECLARE_KEY_ENTRY(VK_7)
	DECLARE_KEY_ENTRY(VK_8)
	DECLARE_KEY_ENTRY(VK_9)
	DECLARE_KEY_ENTRY(VK_A)
	DECLARE_KEY_ENTRY(VK_B)
	DECLARE_KEY_ENTRY(VK_C)
	DECLARE_KEY_ENTRY(VK_D)
	DECLARE_KEY_ENTRY(VK_E)
	DECLARE_KEY_ENTRY(VK_F)
	DECLARE_KEY_ENTRY(VK_G)
	DECLARE_KEY_ENTRY(VK_H)
	DECLARE_KEY_ENTRY(VK_I)
	DECLARE_KEY_ENTRY(VK_J)
	DECLARE_KEY_ENTRY(VK_K)
	DECLARE_KEY_ENTRY(VK_L)
	DECLARE_KEY_ENTRY(VK_M)
	DECLARE_KEY_ENTRY(VK_N)
	DECLARE_KEY_ENTRY(VK_O)
	DECLARE_KEY_ENTRY(VK_P)
	DECLARE_KEY_ENTRY(VK_Q)
	DECLARE_KEY_ENTRY(VK_R)
	DECLARE_KEY_ENTRY(VK_S)
	DECLARE_KEY_ENTRY(VK_T)
	DECLARE_KEY_ENTRY(VK_U)
	DECLARE_KEY_ENTRY(VK_V)
	DECLARE_KEY_ENTRY(VK_W)
	DECLARE_KEY_ENTRY(VK_X)
	DECLARE_KEY_ENTRY(VK_Y)
	DECLARE_KEY_ENTRY(VK_Z)
	DECLARE_KEY_ENTRY(VK_LWIN)
	DECLARE_KEY_ENTRY(VK_RWIN)
	DECLARE_KEY_ENTRY(VK_APPS)
	DECLARE_KEY_ENTRY(VK_SLEEP)
	DECLARE_KEY_ENTRY(VK_NUMPAD0)
	DECLARE_KEY_ENTRY(VK_NUMPAD1)
	DECLARE_KEY_ENTRY(VK_NUMPAD2)
	DECLARE_KEY_ENTRY(VK_NUMPAD3)
	DECLARE_KEY_ENTRY(VK_NUMPAD4)
	DECLARE_KEY_ENTRY(VK_NUMPAD5)
	DECLARE_KEY_ENTRY(VK_NUMPAD6)
	DECLARE_KEY_ENTRY(VK_NUMPAD7)
	DECLARE_KEY_ENTRY(VK_NUMPAD8)
	DECLARE_KEY_ENTRY(VK_NUMPAD9)
	DECLARE_KEY_ENTRY(VK_MULTIPLY)
	DECLARE_KEY_ENTRY(VK_ADD)
	DECLARE_KEY_ENTRY(VK_SEPARATOR)
	DECLARE_KEY_ENTRY(VK_SUBTRACT)
	DECLARE_KEY_ENTRY(VK_DECIMAL)
	DECLARE_KEY_ENTRY(VK_DIVIDE)
	DECLARE_KEY_ENTRY(VK_F1)
	DECLARE_KEY_ENTRY(VK_F2)
	DECLARE_KEY_ENTRY(VK_F3)
	DECLARE_KEY_ENTRY(VK_F4)
	DECLARE_KEY_ENTRY(VK_F5)
	DECLARE_KEY_ENTRY(VK_F6)
	DECLARE_KEY_ENTRY(VK_F7)
	DECLARE_KEY_ENTRY(VK_F8)
	DECLARE_KEY_ENTRY(VK_F9)
	DECLARE_KEY_ENTRY(VK_F10)
	DECLARE_KEY_ENTRY(VK_F11)
	DECLARE_KEY_ENTRY(VK_F12)
	DECLARE_KEY_ENTRY(VK_F13)
	DECLARE_KEY_ENTRY(VK_F14)
	DECLARE_KEY_ENTRY(VK_F15)
	DECLARE_KEY_ENTRY(VK_F16)
	DECLARE_KEY_ENTRY(VK_F17)
	DECLARE_KEY_ENTRY(VK_F18)
	DECLARE_KEY_ENTRY(VK_F19)
	DECLARE_KEY_ENTRY(VK_F20)
	DECLARE_KEY_ENTRY(VK_F21)
	DECLARE_KEY_ENTRY(VK_F22)
	DECLARE_KEY_ENTRY(VK_F23)
	DECLARE_KEY_ENTRY(VK_F24)
	DECLARE_KEY_ENTRY(VK_NUMLOCK)
	DECLARE_KEY_ENTRY(VK_SCROLL)
	DECLARE_KEY_ENTRY(VK_OEM_NEC_EQUAL)
	DECLARE_KEY_ENTRY(VK_OEM_FJ_JISHO)
	DECLARE_KEY_ENTRY(VK_OEM_FJ_MASSHOU)
	DECLARE_KEY_ENTRY(VK_OEM_FJ_TOUROKU)
	DECLARE_KEY_ENTRY(VK_OEM_FJ_LOYA)
	DECLARE_KEY_ENTRY(VK_OEM_FJ_ROYA)
	DECLARE_KEY_ENTRY(VK_LSHIFT)
	DECLARE_KEY_ENTRY(VK_RSHIFT)
	DECLARE_KEY_ENTRY(VK_LCONTROL)
	DECLARE_KEY_ENTRY(VK_RCONTROL)
	DECLARE_KEY_ENTRY(VK_LMENU)
	DECLARE_KEY_ENTRY(VK_RMENU)
	DECLARE_KEY_ENTRY(VK_BROWSER_BACK)
	DECLARE_KEY_ENTRY(VK_BROWSER_FORWARD)
	DECLARE_KEY_ENTRY(VK_BROWSER_REFRESH)
	DECLARE_KEY_ENTRY(VK_BROWSER_STOP)
	DECLARE_KEY_ENTRY(VK_BROWSER_SEARCH)
	DECLARE_KEY_ENTRY(VK_BROWSER_FAVORITES)
	DECLARE_KEY_ENTRY(VK_BROWSER_HOME)
	DECLARE_KEY_ENTRY(VK_VOLUME_MUTE)
	DECLARE_KEY_ENTRY(VK_VOLUME_DOWN)
	DECLARE_KEY_ENTRY(VK_VOLUME_UP)
	DECLARE_KEY_ENTRY(VK_MEDIA_NEXT_TRACK)
	DECLARE_KEY_ENTRY(VK_MEDIA_PREV_TRACK)
	DECLARE_KEY_ENTRY(VK_MEDIA_STOP)
	DECLARE_KEY_ENTRY(VK_MEDIA_PLAY_PAUSE)
	DECLARE_KEY_ENTRY(VK_LAUNCH_MAIL)
	DECLARE_KEY_ENTRY(VK_LAUNCH_MEDIA_SELECT)
	DECLARE_KEY_ENTRY(VK_LAUNCH_APP1)
	DECLARE_KEY_ENTRY(VK_LAUNCH_APP2)
	DECLARE_KEY_ENTRY(VK_OEM_1)
	DECLARE_KEY_ENTRY(VK_OEM_PLUS)
	DECLARE_KEY_ENTRY(VK_OEM_COMMA)
	DECLARE_KEY_ENTRY(VK_OEM_MINUS)
	DECLARE_KEY_ENTRY(VK_OEM_PERIOD)
	DECLARE_KEY_ENTRY(VK_OEM_2)
	DECLARE_KEY_ENTRY(VK_OEM_3)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_A)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_B)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_X)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_Y)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_RIGHT_SHOULDER)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_LEFT_SHOULDER)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_LEFT_TRIGGER)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_RIGHT_TRIGGER)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_DPAD_UP)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_DPAD_DOWN)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_DPAD_LEFT)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_DPAD_RIGHT)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_MENU)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_VIEW)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_LEFT_THUMBSTICK_UP)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_LEFT_THUMBSTICK_DOWN)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_LEFT_THUMBSTICK_LEFT)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_RIGHT_THUMBSTICK_UP)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT)
	DECLARE_KEY_ENTRY(VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT)
	DECLARE_KEY_ENTRY(VK_OEM_4)
	DECLARE_KEY_ENTRY(VK_OEM_5)
	DECLARE_KEY_ENTRY(VK_OEM_6)
	DECLARE_KEY_ENTRY(VK_OEM_7)
	DECLARE_KEY_ENTRY(VK_OEM_8)
	DECLARE_KEY_ENTRY(VK_OEM_AX)
	DECLARE_KEY_ENTRY(VK_OEM_102)
	DECLARE_KEY_ENTRY(VK_ICO_HELP)
	DECLARE_KEY_ENTRY(VK_ICO_00)
	DECLARE_KEY_ENTRY(VK_PROCESSKEY)
	DECLARE_KEY_ENTRY(VK_ICO_CLEAR)
	DECLARE_KEY_ENTRY(VK_PACKET)
	DECLARE_KEY_ENTRY(VK_OEM_RESET)
	DECLARE_KEY_ENTRY(VK_OEM_JUMP)
	DECLARE_KEY_ENTRY(VK_OEM_PA1)
	DECLARE_KEY_ENTRY(VK_OEM_PA2)
	DECLARE_KEY_ENTRY(VK_OEM_PA3)
	DECLARE_KEY_ENTRY(VK_OEM_WSCTRL)
	DECLARE_KEY_ENTRY(VK_OEM_CUSEL)
	DECLARE_KEY_ENTRY(VK_OEM_ATTN)
	DECLARE_KEY_ENTRY(VK_OEM_FINISH)
	DECLARE_KEY_ENTRY(VK_OEM_COPY)
	DECLARE_KEY_ENTRY(VK_OEM_AUTO)
	DECLARE_KEY_ENTRY(VK_OEM_ENLW)
	DECLARE_KEY_ENTRY(VK_OEM_BACKTAB)
	DECLARE_KEY_ENTRY(VK_ATTN)
	DECLARE_KEY_ENTRY(VK_CRSEL)
	DECLARE_KEY_ENTRY(VK_EXSEL)
	DECLARE_KEY_ENTRY(VK_EREOF)
	DECLARE_KEY_ENTRY(VK_PLAY)
	DECLARE_KEY_ENTRY(VK_ZOOM)
	DECLARE_KEY_ENTRY(VK_NONAME)
	DECLARE_KEY_ENTRY(VK_PA1)
	DECLARE_KEY_ENTRY(VK_OEM_CLEAR)
};