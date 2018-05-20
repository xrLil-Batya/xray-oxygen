#pragma once
#include "../xrCore/net_utils.h"


ENGINE_API extern ClientID BroadcastCID;

IC u32 TimeGlobal	(CTimer* timer)	{ return timer->GetElapsed_ms();	}
IC u32 TimerAsync	(CTimer* timer) { return TimeGlobal	(timer);		}
