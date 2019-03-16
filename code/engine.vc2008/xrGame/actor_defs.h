#pragma once
#include "PHSynchronize.h"
#include "xrserver_space.h"

#define ACTOR_HEIGHT			1.75f
#define ACTOR_LOOKOUT_SPEED		2.f

namespace ACTOR_DEFS 
{

enum ESoundCcount {
//	SND_HIT_COUNT=8,
	SND_DIE_COUNT=4
};

enum EDamages {DAMAGE_FX_COUNT = 12};


enum EMoveCommand
{
	mcFwd		= (1ul<<0ul),
	mcBack		= (1ul<<1ul),
	mcLStrafe	= (1ul<<2ul),
	mcRStrafe	= (1ul<<3ul),
	mcCrouch	= (1ul<<4ul),
	mcAccel		= (1ul<<5ul),
	mcTurn		= (1ul<<6ul),
	mcJump		= (1ul<<7ul),
	mcFall		= (1ul<<8ul),
	mcLanding	= (1ul<<9ul),
	mcLanding2	= (1ul<<10ul),
	mcClimb		= (1ul<<11ul),
	mcSprint	= (1ul<<12ul),
	mcLLookout	= (1ul<<13ul),
	mcRLookout	= (1ul<<14ul),
	mcAnyMove	= (mcFwd|mcBack|mcLStrafe|mcRStrafe),
	mcAnyAction = (mcAnyMove|mcJump|mcFall|mcLanding|mcLanding2), //mcTurn|
	mcAnyState	= (mcCrouch|mcAccel|mcClimb|mcSprint),
	mcLookout	= (mcLLookout|mcRLookout),
};

// enum для определения действия над вещью на которую наведен в текущее время прицел.
// Используется для показа всплывающих динамических подсказок
enum EActorAction
{
	eaaNoAction			= 0,
	eaaPickup,
	eaaTalk,
	eaaOpenDoor,
	eaaSearchCorpse,
};

using EActorSleep = const char*;
extern EActorSleep		easCanSleepResult;
extern 	string32		g_quick_use_slots[4];
}; // namespace ACTOR_DEFS 

