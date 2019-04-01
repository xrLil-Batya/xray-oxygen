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
		u32 value = NewTime->Days * 86400 + NewTime->Hours * 3600 + NewTime->Minutes * 60;
		float fValue = static_cast<float> (value);
		value *= 1000;//msec		
		::Environment().ChangeGameTime(fValue);
		::Level().Server->game->alife().time_manager().change_game_time(value);
	}
}


void XRay::Game::SGameTime::Years::set(::System::UInt32 value)
{
	_years = value;
}

void XRay::Game::SGameTime::Months::set(::System::UInt32 value)
{
	_months = value;
}

void XRay::Game::SGameTime::Weeks::set(::System::UInt32 value)
{
	_weeks = value;
}

void XRay::Game::SGameTime::Days::set(::System::UInt32 value)
{
	_days = value;
}

void XRay::Game::SGameTime::Hours::set(::System::UInt32 value)
{
	_hours = value;
}

void XRay::Game::SGameTime::Minutes::set(::System::UInt32 value)
{
	_minutes = value;
}

void XRay::Game::SGameTime::Seconds::set(::System::UInt32 value)
{
	_seconds = value;
}

void XRay::Game::SGameTime::Miliseconds::set(::System::UInt32 value)
{
	_miliseconds = value;
}

::System::UInt32 XRay::Game::SGameTime::Years::get()
{
	return _years;
}

::System::UInt32 XRay::Game::SGameTime::Months::get()
{
	return _months;
}

::System::UInt32 XRay::Game::SGameTime::Weeks::get()
{
	return _weeks;
}

::System::UInt32 XRay::Game::SGameTime::Days::get()
{
	return _days;
}

::System::UInt32 XRay::Game::SGameTime::Hours::get()
{
	return _hours;
}

::System::UInt32 XRay::Game::SGameTime::Minutes::get()
{
	return _minutes;
}

::System::UInt32 XRay::Game::SGameTime::Seconds::get()
{
	return _seconds;
}

::System::UInt32 XRay::Game::SGameTime::Miliseconds::get()
{
	return _miliseconds;
}

void XRay::Game::SGameTime::ChangeGameTime(u32 days, u32 hours, u32 mins)
{
	if (::Level().Server->game && ai().get_alife())
	{
		u32 value = days * 86400 + hours * 3600 + mins * 60;
		float fValue = static_cast<float> (value);
		value *= 1000; //msec		
		::Environment().ChangeGameTime(fValue);
		::Level().Server->game->alife().time_manager().change_game_time(value);
	}
}

void XRay::Game::GameDifficulty::set(ESingleGameDifficulty dif)
{
	g_SingleGameDifficulty = (::ESingleGameDifficulty)u32(dif);
	::Actor()->OnDifficultyChanged();
}

XRay::ESingleGameDifficulty XRay::Game::GameDifficulty::get()
{
	return (ESingleGameDifficulty)u32(g_SingleGameDifficulty);
}

u64 XRay::Game::StartTime::get()
{
	return ::Level().GetStartGameTime();
}

float XRay::Game::SndVolume::get()
{
	return psSoundVFactor;
}

void XRay::Game::SndVolume::set(float v)
{
	psSoundVFactor = v;
	clamp(psSoundVFactor, 0.0f, 1.0f);
}