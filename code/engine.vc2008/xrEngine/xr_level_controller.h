#pragma once

enum EGameActions
{
	kLEFT,
	kRIGHT,
	kUP,
	kDOWN,
	kJUMP,
	kCROUCH,
	kACCEL,
	kSPRINT_TOGGLE,
						
	kFWD,
	kBACK,
	kL_STRAFE,
	kR_STRAFE,
						
	kL_LOOKOUT,
	kR_LOOKOUT,
						
	kCAM_1,
	kCAM_2,
	kCAM_3,
	kCAM_ZOOM_IN,
	kCAM_ZOOM_OUT,
						
	kTORCH,
	kNIGHT_VISION,
	kDETECTOR,
	kTURN_ENGINE,
	kSWITCH_HORN,
	kWPN_1,
	kWPN_2,
	kWPN_3,
	kWPN_4,
	kWPN_5,
	kWPN_6,
	kWPN_NEXT,
	kWPN_FIRE,
	kWPN_ZOOM,
	kWPN_RELOAD,
	kWPN_FUNC,
	kWPN_FIREMODE_PREV,
	kWPN_FIREMODE_NEXT,
	kWPN_UN_MIS,
	kWPN_VIEW,
						
	kPAUSE,
	kDROP,
	kUSE,
	kSCORES,
	kSCREENSHOT,
	kQUIT,
	kCONSOLE,
	kINVENTORY,
	kACTIVE_JOBS,
						
	kNEXT_SLOT,
	kPREV_SLOT,
						
	kQUICK_USE_1,
	kQUICK_USE_2,		
	kQUICK_USE_3,		
	kQUICK_USE_4,		
	
	kQUICK_SAVE,
	kQUICK_LOAD,

	kDEV_NOCLIP,
	kDEV_ACTION1,
	kDEV_ACTION2,
	kDEV_ACTION3,
	kDEV_ACTION4,
	
	kLASTACTION,
	kNOTBINDED,
	kFORCEDWORD		= u32(-1)
};

enum eVK : u8
{
	VK_0 = 0x30,
	VK_1,
	VK_2,
	VK_3,
	VK_4,
	VK_5,
	VK_6,
	VK_7,
	VK_8,
	VK_9,
	VK_A = 0x41,
	VK_B = 0x42,
	VK_C = 0x43,
	VK_D = 0x44,
	VK_E = 0x45,
	VK_F = 0x46,
	VK_G = 0x47,
	VK_H = 0x48,
	VK_I = 0x49,
	VK_J = 0x4A,
	VK_K = 0x4B,
	VK_L = 0x4C,
	VK_M = 0x4D,
	VK_N = 0x4E,
	VK_O = 0x4F,
	VK_P = 0x50,
	VK_Q = 0x51,
	VK_R = 0x52,
	VK_S = 0x53,
	VK_T = 0x54,
	VK_U = 0x55,
	VK_V = 0x56,
	VK_W = 0x57,
	VK_X = 0x58,
	VK_Y = 0x59,
	VK_Z = 0x5A,

	// gamepad keys
	VK_GAMEPAD_A                         = 0xC3,
	VK_GAMEPAD_B                         = 0xC4,
	VK_GAMEPAD_X                         = 0xC5,
	VK_GAMEPAD_Y                         = 0xC6,
	VK_GAMEPAD_RIGHT_SHOULDER            = 0xC7,
	VK_GAMEPAD_LEFT_SHOULDER             = 0xC8,
	VK_GAMEPAD_LEFT_TRIGGER              = 0xC9,
	VK_GAMEPAD_RIGHT_TRIGGER             = 0xCA,
	VK_GAMEPAD_DPAD_UP                   = 0xCB,
	VK_GAMEPAD_DPAD_DOWN                 = 0xCC,
	VK_GAMEPAD_DPAD_LEFT                 = 0xCD,
	VK_GAMEPAD_DPAD_RIGHT                = 0xCE,
	VK_GAMEPAD_MENU                      = 0xCF,
	VK_GAMEPAD_VIEW                      = 0xD0,
	VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON    = 0xD1,
	VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON   = 0xD2,
	VK_GAMEPAD_LEFT_THUMBSTICK_UP        = 0xD3,
	VK_GAMEPAD_LEFT_THUMBSTICK_DOWN      = 0xD4,
	VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT     = 0xD5,
	VK_GAMEPAD_LEFT_THUMBSTICK_LEFT      = 0xD6,
	VK_GAMEPAD_RIGHT_THUMBSTICK_UP       = 0xD7,
	VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN     = 0xD8,
	VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT    = 0xD9,
	VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT     = 0xDA,
};

struct _keyboard		
{
	LPCSTR		key_name;
	u8			dik;
	xr_string	key_local_name;
};

struct _action
{
	LPCSTR			action_name;
	EGameActions	id;
};

ENGINE_API const char*	VK_to_keyname			(u8 _dik);
ENGINE_API u8 keyname_to_dik (LPCSTR _name);
ENGINE_API _keyboard*	keyname_to_ptr			(LPCSTR _name);
ENGINE_API _keyboard*	VK_to_ptr				(u8 _dik, bool bSafe);

ENGINE_API const char*	id_to_action_name		(EGameActions _id);
ENGINE_API EGameActions	action_name_to_id		(LPCSTR _name);
ENGINE_API _action*		action_name_to_ptr		(LPCSTR _name);

extern ENGINE_API _action actions[];
extern const xr_map<u8, xr_string> KeyNamesTable;

#define bindings_count kLASTACTION
struct _binding
{
	_action*		m_action;
	_keyboard*		m_keyboard[2];
};

extern ENGINE_API _binding g_key_bindings[];

ENGINE_API bool			is_binded			(EGameActions action_id, u8 dik);
ENGINE_API int			get_action_dik		(EGameActions action_id, int idx=-1);
ENGINE_API EGameActions get_binded_action (u8 dik);
ENGINE_API void			CCC_RegisterInput();

ICF		   bool			isMouseButton(u8 vKey)
{
	return (vKey == VK_LBUTTON || vKey == VK_RBUTTON || vKey == VK_MBUTTON);
}

struct _conCmd	
{
	shared_str	cmd;
};

class ENGINE_API ConsoleBindCmds
{
public:
	xr_map<u8,_conCmd>		m_bindConsoleCmds;

	void 	bind			(u8 dik, LPCSTR N);
	void 	unbind			(u8 dik);
	bool 	execute			(u8 dik);
	void 	clear			();
	void 	save			(IWriter* F);
};

ENGINE_API void GetActionAllBinding	(LPCSTR action, char* dst_buff, int dst_buff_sz);

extern ENGINE_API ConsoleBindCmds bindConsoleCmds;
