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

std::string Time::GetSeconds()
{
	return (aTm->tm_sec < 10) ?
		"0" + std::to_string(aTm->tm_sec) :
		std::to_string(aTm->tm_sec);
}

std::string Time::GetMinutes()
{
	return (aTm->tm_min < 10) ?
		"0" + std::to_string(aTm->tm_min) :
		std::to_string(aTm->tm_min);
}

std::string Time::GetHours()
{
	return (aTm->tm_hour < 10) ?
		"0" + std::to_string(aTm->tm_hour) :
		std::to_string(aTm->tm_hour);
}

std::string Time::GetDay()
{
	return (aTm->tm_mday < 10) ? 
		"0" + std::to_string(aTm->tm_mday) : 
		std::to_string(aTm->tm_mday);
}

std::string Time::GetMonth()
{
	return (aTm->tm_mon + 1 < 10) ? 
		"0" + std::to_string(aTm->tm_mon + 1) : 
		std::to_string(aTm->tm_mon + 1);
}

std::string Time::GetYear()
{
	return std::to_string(aTm->tm_year + 1900);
}