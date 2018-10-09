#pragma once

enum {
		AF_GODMODE				=(1<<0),
		AF_NO_CLIP				=(1<<1),
		AF_UNLIMITEDAMMO		=(1<<3),
		AF_RUN_BACKWARD			=(1<<4),
		AF_AUTOPICKUP			=(1<<5),
		AF_PSP					=(1<<6),
		AF_DYNAMIC_MUSIC		=(1<<7),
		AF_IMPORTANT_SAVE		=(1<<9),
		AF_CROUCH_TOGGLE		=(1<<10),
		//AF_RELOADONSPRINT		=(1<<11),
		AF_CUR_INS_CROS			=(1<<12),
		AF_COLORED_FEEL			=(1<<13),
		AF_SHOW_CURPOS			=(1<<14),
		AF_ZOOM_NEW_FD			=(1<<15),
		AF_CAR_INFO				=(1<<16),
		AF_HARDCORE				=(1<<17),
		AF_WORKINPROGRESS		=(1<<18),
        AF_CLEARSKYINTERFACE	=(1<<19),
        AF_SHOWDATE				=(1<<20),
		AF_GET_OBJECT_PARAMS	=(1<<21),
		AF_SHOW_BOSS_HEALTH		=(1<<22),
		AF_RIGHT_SHOULDER		=(1<<23),
		AF_FP2ZOOM_FORCED		=(1<<24),
};

extern ENGINE_API Flags32	psActorFlags;
extern BOOL		GodMode	();	