#include "stdafx.h"
#include "FTimer.h"

XRCORE_API bool g_bEnableStatGather = false;
XRCORE_API pauseMngr g_pauseMngr;

void CStatTimer::FrameStart()
{
    accum = 0;
    count = 0;
}

void CStatTimer::FrameEnd()
{
	double Time = 1000.0 * double(accum) / double(CPU::qpc_freq);
    if (Time > result) result = Time;
    else result = 0.99 * result + 0.01* Time;
}

ScopeStatTimer::ScopeStatTimer(CStatTimer& destTimer) : _timer(destTimer)
{
	_timer.Begin();
}

ScopeStatTimer::~ScopeStatTimer()
{
	_timer.End();
}
