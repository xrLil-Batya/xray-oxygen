#pragma once

#ifdef	GAME_BUILD
#define GAME_API		__declspec(dllexport)
#else
#define GAME_API		__declspec(dllimport)
#endif