////////////////////////////////////////////////////////////////////////////
//	Module 		: date_time.h
//	Created 	: 25.08.2017
//	Author		: ForserX
//	Description : Date and time routines
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
inline u32 extra_day_count(u32 years) { return ((years % 400 == 0) || ((years % 4 == 0) && (years % 100 != 0))) ? 1 : 0; }
static int days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

inline u64 generate_time_impl(u32 years, u32 months, u32 days, u32 hours, u32 minutes, u32 seconds, u32 milliseconds)
{
	u64 const years_minus_1 = u64(years - 1);
	u64	result = years_minus_1 * 365 + years_minus_1 / 4 - years_minus_1 / 100 + years_minus_1 / 400;
	for (int it = 0; it < months; it++)
	{
		if (months > it + 1)
		{
			int debug = days_in_month[it];
			result += u64(debug);
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
	result *= 1000ul;
	result += u64(milliseconds);
	return result;
}

inline void split_time_impl(u64 time, u32 &years, u32 &months, u32 &days, u32 &hours, u32 &minutes, u32 &seconds, u32 &milliseconds)
{
	milliseconds = u32(time % 1000);
	time /= 1000;
	seconds = u32(time % 60);
	time /= 60;
	minutes = u32(time % 60);
	time /= 60;
	hours = u32(time % 24);
	time /= 24;

	u64 const p0 = time / (400 * 365 + 100 - 4 + 1);
	time -= p0*(400 * 365 + 100 - 4 + 1);
	u64 const p1 = time / (100 * 365 + 25 - 1);
	time -= p1*(100 * 365 + 25 - 1);
	u64 const p2 = time / (4 * 365 + 1);
	time -= p2*(4 * 365 + 1);
	u64 const p3 = std::min(u32(time) / 365, (u32)3);
	time -= p3 * 365;
	years = u32(400 * p0 + 100 * p1 + 4 * p2 + p3 + 1);
	++time;

	months = 1;
	for (int it = 0; it < months; it++)
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

u64	generate_time				(u32 years, u32 months, u32 days, u32 hours, u32 minutes, u32 seconds, u32 milliseconds)
{
	u64 const result			= generate_time_impl(years, months, days, hours, minutes, seconds, milliseconds);
	return						(result);
}

void split_time					(u64 time, u32& years, u32& months, u32& days, u32& hours, u32& minutes, u32& seconds, u32& milliseconds)
{
	split_time_impl				(time, years, months, days, hours, minutes, seconds, milliseconds);
}