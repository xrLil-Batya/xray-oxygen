#include "stdafx.h"
#include "DiscordRichPresense.h"
#include "DiscordRichPresense/discord_register.h"
#include "DiscordRichPresense/discord_rpc.h"
#include "x_ray.h"

ENGINE_API xrDiscordPresense g_discord;

void xrDiscordPresense::Initialize()
{
	// We don't have multiplayer mode, so no need to invite system to support
	DiscordEventHandlers nullHandlers;
	ZeroMemory(&nullHandlers, sizeof(nullHandlers));
	Discord_Initialize("503315994324762625", &nullHandlers, TRUE, nullptr);
	bInitialize = true;
}

void xrDiscordPresense::Shutdown()
{
	if (bInitialize)
	{
		Discord_Shutdown();
		bInitialize = false;
	}
}

void xrDiscordPresense::SetStatus(StatusId status)
{
	if (!bInitialize) return;

	DiscordRichPresence presenseInfo;
	ZeroMemory(&presenseInfo, sizeof(presenseInfo));

	StatusId realStatus = status;
	if (status == StatusId::In_Game)
	{
		// get level name, and set status to different value, when we found vanilla levels
		if (pApp->Level_Current < pApp->Levels.size())
		{
			CApplication::sLevelInfo& LevelInfo = pApp->Levels[pApp->Level_Current];
			if (xr_strcmp(LevelInfo.name, "zaton") == 0)
			{
				realStatus = StatusId::Zaton;
			}
			if (xr_strcmp(LevelInfo.name, "jupiter") == 0)
			{
				realStatus = StatusId::Upiter;
			}
			if (xr_strcmp(LevelInfo.name, "pripyat") == 0)
			{
				realStatus = StatusId::Pripyat;
			}
		}
	}

	presenseInfo.largeImageText = "Zone Awaits...";

	switch (realStatus)
	{
	case StatusId::In_Game:
		presenseInfo.details		= "In Game";
		presenseInfo.largeImageKey	= "zaton";
		break;
	case StatusId::Zaton:
		presenseInfo.details		= "In Game (Zaton)";
		presenseInfo.largeImageKey  = "zaton";
		break;
	case StatusId::Upiter:
		presenseInfo.details		= "In Game (Jupiter)";
		presenseInfo.largeImageKey  = "ypiter";
		break;
	case StatusId::Pripyat:
		presenseInfo.details		= "In Game (Pripyat)";
		presenseInfo.largeImageKey  = "pripyat";
		break;
	default:
	case StatusId::Menu:
		presenseInfo.details		= "In Menu";
		presenseInfo.largeImageKey	= "menu_final";
		break;
	}

	Discord_UpdatePresence(&presenseInfo);
}

xrDiscordPresense::~xrDiscordPresense()
{
	Shutdown();
}
