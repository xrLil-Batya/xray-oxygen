#include "stdafx.h"
#include "Game.h"
#include "../xrEngine/date_time.h"
#include "../xrGame/Level.h"
#include "../xrGame/ai_space.h"
#include "../xrGame/alife_simulator.h"
#include "../xrGame/alife_time_manager.h"

XRay::Game::SGameTime^ XRay::Game::Time::get()
{
	u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
	split_time((g_pGameLevel && ::Level().game) ? ::Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);

	SGameTime^ CurTime = gcnew SGameTime();

	CurTime->Years = year;
	CurTime->Months = month;
	//CurTime->Weeks = week;
	CurTime->Days = day;
	CurTime->Hours = hours;
	CurTime->Minutes = mins;
	CurTime->Seconds = secs;
//	CurTime->Miliseconds = milisecs;

	return CurTime;
}

void XRay::Game::Time::set(XRay::Game::SGameTime^ NewTime)
{
	if (::Level().Server->game && ai().get_alife())
	{
	//	u32 value = NewTime->Days * 86400 + NewTime->Hours * 3600 + NewTime->Minutes * 60;
	//	float fValue = static_cast<float> (value);
	//	value *= 1000;//msec		
	//	Environment().ChangeGameTime(fValue);
	//	::Level().Server->game->alife().time_manager().change_game_time(value);
	}
}


void XRay::Game::SGameTime::Years::set(::System::UInt32 days)
{

}

void XRay::Game::SGameTime::Months::set(::System::UInt32 days)
{

}

void XRay::Game::SGameTime::Weeks::set(::System::UInt32 days)
{

}

void XRay::Game::SGameTime::Days::set(::System::UInt32 days)
{

}

void XRay::Game::SGameTime::Hours::set(::System::UInt32 hours)
{

}

void XRay::Game::SGameTime::Minutes::set(::System::UInt32 minutes)
{

}

void XRay::Game::SGameTime::Seconds::set(::System::UInt32 seconds)
{

}

void XRay::Game::SGameTime::ChangeGameTime(u32 days, u32 hours, u32 mins)
{
	if (::Level().Server->game && ai().get_alife())
	{
		u32 value = days * 86400 + hours * 3600 + mins * 60;
		float fValue = static_cast<float> (value);
		value *= 1000; //msec		
		Environment().ChangeGameTime(fValue);
		::Level().Server->game->alife().time_manager().change_game_time(value);
	}
}