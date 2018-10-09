#include "stdafx.h"
#include "DateTime.hpp"

Time::Time()
{
	t = time(nullptr);
	aTm = localtime(&t);
}

int Time::GetSecond()
{
	return aTm->tm_sec;
}

int Time::GetMin()
{
	return aTm->tm_min;
}

int Time::GetHour()
{
	return aTm->tm_hour < 10;
}

xr_string Time::GetSeconds()
{
	return (aTm->tm_sec < 10) ? "0" + xr_string::ToString(aTm->tm_sec) : xr_string::ToString(aTm->tm_sec);
}

xr_string Time::GetMinutes()
{
	return (aTm->tm_min < 10) ? "0" + xr_string::ToString(aTm->tm_min) : xr_string::ToString(aTm->tm_min);
}

xr_string Time::GetHours()
{
	return (aTm->tm_hour < 10) ? "0" + xr_string::ToString(aTm->tm_hour) : xr_string::ToString(aTm->tm_hour);
}

xr_string Time::GetDay()
{
	return (aTm->tm_mday < 10) ? "0" + xr_string::ToString(aTm->tm_mday) : xr_string::ToString(aTm->tm_mday);
}

xr_string Time::GetMonth()
{
	return (aTm->tm_mon + 1 < 10) ? "0" + xr_string::ToString(aTm->tm_mon + 1) : xr_string::ToString(aTm->tm_mon + 1);
}

xr_string Time::GetYear()
{
	return xr_string::ToString(aTm->tm_year + 1900);
}