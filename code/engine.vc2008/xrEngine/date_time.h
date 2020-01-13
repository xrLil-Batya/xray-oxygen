////////////////////////////////////////////////////////////////////////////
//	Module 		: date_time.h
//	Created 	: 23.05.2018
//	Author		: ForserX
//	Description : Ingame date and time generator. 
////////////////////////////////////////////////////////////////////////////
#pragma once

#define TIMETYPE_MILISECONDS	0x1
#define TIMETYPE_SECONDS		0x2
#define TIMETYPE_MINUTES		0x3
#define TIMETYPE_HOURS			0x4
#define TIMETYPE_DAYS			0x5
#define TIMETYPE_WEEKS			0x6
#define TIMETYPE_MONTHS			0x7
#define TIMETYPE_YEARS			0x8

namespace GameTime
{
ENGINE_API	u64		generate_time(u32 years, u32 months, u32 days, u32 hours, u32 minutes, u32 seconds, u32 milliseconds = 0);
ENGINE_API	void	split_time(u64 time, u32 &years, u32 &months, u32 &days, u32 &hours, u32 &minutes, u32 &seconds, u32 &milliseconds);
ENGINE_API	u32		return_time(u64 time, u8 timeType );
			u32		extra_day_count(u32 years);
}

// Мне лень везде писать название namespace в вызовах.
using namespace GameTime;
