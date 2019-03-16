#include "stdafx.h"
#include "IGame_Actor.h"
#include "../xrGame/Actor_Flags.h"

ENGINE_API CIActor* g_actor = nullptr;
ENGINE_API int psActorSleepTime = 1;
ENGINE_API string_path g_last_saved_game;
ENGINE_API Flags32	psActorFlags = { AF_AUTOPICKUP | AF_RUN_BACKWARD | AF_IMPORTANT_SAVE | AF_SHOWDATE | AF_GET_OBJECT_PARAMS | AF_SHOW_BOSS_HEALTH };

void CIActor::cam_Set(EActorCameras style)
{
	CCameraBase* old_cam = cam_Active();
	cam_active = style;
	old_cam->OnDeactivate();
	cam_Active()->OnActivate(old_cam);
}