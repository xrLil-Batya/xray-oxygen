#pragma once
#include "../xrCore/net_utils.h"
#include <dplay/dplay8.h>
#include "net_messages.h"


ENGINE_API extern ClientID BroadcastCID;

IC u32 TimeGlobal	(CTimer* timer)	{ return timer->GetElapsed_ms();	}
IC u32 TimerAsync	(CTimer* timer) { return TimeGlobal	(timer);		}

class ENGINE_API IClientStatistic
{
	CTimer*				device_timer;
public:
			IClientStatistic	(CTimer* timer){ std::memset(this,0,sizeof(*this)); device_timer=timer; }

	
	IC void	Clear				()	{ CTimer* timer = device_timer; std::memset(this,0,sizeof(*this)); device_timer=timer; }

};

