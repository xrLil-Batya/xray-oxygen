#include "stdafx.h"
#include "IGame_Actor.h"

ENGINE_API CIActor* g_actor = new CIActor();
ENGINE_API int psActorSleepTime = 1;
ENGINE_API Flags32	psActorFlags;
ENGINE_API string_path g_last_saved_game;