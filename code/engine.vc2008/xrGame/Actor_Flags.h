#pragma once

enum{
		AF_GODMODE			=(1<<0),
		AF_NO_CLIP			=(1<<1),
		AF_UNLIMITEDAMMO	=(1<<3),
		AF_RUN_BACKWARD		=(1<<4),
		AF_AUTOPICKUP		=(1<<5),
		AF_PSP				=(1<<6),
		AF_DYNAMIC_MUSIC	=(1<<7),
		AF_GODMODE_RT		=(1<<8),
		AF_IMPORTANT_SAVE	=(1<<9),
		AF_CROUCH_TOGGLE	=(1<<10),
		AF_RELOADONSPRINT	=(1<<11),
		AF_CUR_INS_CROS     =(1<<12),
		AF_COLORED_FEEL     =(1<<13),
		AF_SHOW_CURPOS		=(1<<14),
		AF_ZOOM_NEW_FD		=(1<<15),
		AF_CAR_INFO			=(1<<16),
		AF_HARDCORE			=(1<<17),
		AF_WORKINPROGRESS	=(1<<18),
        AF_CLEARSKYINTERFACE=(1<<19),
        AF_SHOWDATE         =(1<<20),
};

extern Flags32	psActorFlags;
extern BOOL		GodMode	();	

extern int		psActorSleepTime;

