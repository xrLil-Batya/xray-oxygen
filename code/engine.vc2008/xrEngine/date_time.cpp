////////////////////////////////////////////////////////////////////////////
//	Module 		: date_time.h
//	Created 	: 25.08.2017
//	Author		: ForserX
//	Description : Ingame date and time generator. 
////////////////////////////////////////////////////////////////////////////
// Oxygen Engine: 2016-2019
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "date_time.h"

u32 GameTime::extra_day_count(u32 years)
{ 
	return ((years % 400 == 0) || ((years % 4 == 0) && (years % 100 != 0))) ? 1 : 0; 
}

static u32 days_in_month[12] = { 31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u };

ENGINE_API u64 GameTime::generate_time(u32 years, u32 months, u32 days, u32 hours, u32 minutes, u32 seconds)
{
	u64 const years_minus_1 = u64(years - 1);
	u64	result = years_minus_1 * 365 + years_minus_1 / 4 - years_minus_1 / 100 + years_minus_1 / 400;
	for (u32 it = 0; it < months; it++)
	{
		if (months > it + 1)
		{
			result += days_in_month[it];
			if (it == 1)
			{
				result += extra_day_count(years);
			}
		}
	}

	result += u64(days - 1);
	result *= 24ul;
	result += u64(hours);
	result *= 60ul;
	result += u64(minutes);
	result *= 60ul;
	result += u64(seconds);
	return result;
}

ENGINE_API void GameTime::split_time(u64 time, u32 &years, u32 &months, u32 &days, u32 &hours, u32 &minutes, u32 &seconds)
{
	seconds = u32(time % 60u);
	time /= 60;
	minutes = u32(time % 60u);
	time /= 60;
	hours = u32(time % 24u);
	time /= 24;

	u64 const p0 = time / (400 * 365 + 100 - 4 + 1);
	time -= p0*(400 * 365 + 100 - 4 + 1);
	u64 const p1 = time / (100 * 365 + 25 - 1);
	time -= p1*(100 * 365 + 25 - 1);
	u64 const p2 = time / (4 * 365 + 1);
	time -= p2*(4 * 365 + 1);
	u64 const p3 = std::min(time / 365u, 3ui64);
	time -= p3 * 365;
	years = u32(400 * p0 + 100 * p1 + 4 * p2 + p3 + 1);
	++time;

	months = 1;
	for (u32 it = 0; it < months; it++)
	{
		if ((it == 1) && days_in_month[it] + extra_day_count(years) < time)
		{
			++months;
			time -= days_in_month[it] + extra_day_count(years);
		}
		else if (days_in_month[it] < time)
		{
			++months;
			time -= days_in_month[it];
		}
	}
	days = u32(time);
}

u32 return_time(u64 time, u8 timeType)
{
	u32 seconds, minutes, hours, day, months, years;
	split_time(time, years, months, day, hours, minutes, seconds);

	switch (timeType)
	{
		case TIMETYPE_SECONDS:		return seconds; break;
		case TIMETYPE_MINUTES:		return minutes; break;
		case TIMETYPE_DAYS:			return day; break;
		case TIMETYPE_MONTHS:		return months; break;
		case TIMETYPE_YEARS: 		return years; break;
		
		default: 					return 0;
	}
}