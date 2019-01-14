#include "stdafx.h"
#include "Level.h"
#include "../xrGame/ai_space.h"
#include "../xrGame/level_graph.h"
#include "../xrGame/Level.h"
#include "../xrGame/Actor.h"
#include "../xrGame/alife_simulator.h"
#include "../xrGame/alife_time_manager.h"
#include "../xrEngine/date_time.h"

u32 XRay::LevelGraph::LevelID::get()
{
	return ai().level_graph().level_id();
}

u32 XRay::LevelGraph::VertexCount::get()
{
	return  ai().level_graph().header().vertex_count();
}

System::String^ XRay::Level::LevelName::get()
{
	return gcnew ::System::String(::Level().name_translated().c_str());
}

System::String^ XRay::Level::Weather::get()
{
	return gcnew ::System::String(::Environment().GetWeather().c_str());
}

void XRay::Level::Weather::set(::System::String^ str)
{
	if (!Device.editor())
	{
		string128 WetNameStr = { 0 };
		ConvertDotNetStringToAscii(str, WetNameStr);
		::Environment().SetWeather(WetNameStr, false);
	}
}

void XRay::Level::SetWeatherFX(::System::String^ str)
{
	if (!Device.editor())
	{
		string128 WetNameStr = { 0 };
		ConvertDotNetStringToAscii(str, WetNameStr);
		::Environment().SetWeatherFX(WetNameStr);
	}
}	

void XRay::Level::StartWeatherFXfromTime(::System::String^ str, float time)
{
	if (!Device.editor())
	{
		string128 WetNameStr = { 0 };
		ConvertDotNetStringToAscii(str, WetNameStr);
		::Environment().StartWeatherFXFromTime(WetNameStr, time);
	}
}

bool XRay::Level::is_wfx_playing()
{
	return (::Environment().IsWeatherFXPlaying());
}

float XRay::Level::Get_wfx_time()
{
	return (::Environment().wfx_time);
}

void XRay::Level::Stop_weather_fx()
{
	(::Environment().StopWeatherFX());
}

void XRay::Level::TimeFactor::set(float time_factor)
{
	if (Device.editor())
		return;
	(::Level().Server->game->SetGameTimeFactor(time_factor));
}

float XRay::Level::TimeFactor::get()
{
	return (::Level().GetGameTimeFactor());
}

void XRay::Level::GameDifficulty::set(ESingleGameDifficulty dif)
{
	g_SingleGameDifficulty = (::ESingleGameDifficulty)u32(dif);
	Actor()->OnDifficultyChanged();
}

XRay::ESingleGameDifficulty XRay::Level::GameDifficulty::get()
{
	return (ESingleGameDifficulty)u32(g_SingleGameDifficulty);
}

u32 XRay::Level::Get_time_days()
{
	u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
	split_time((g_pGameLevel && ::Level().game) ? ::Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);
	return			day;
}

u32 XRay::Level::Get_time_hours()
{
	u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
	split_time((g_pGameLevel && ::Level().game) ? ::Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);
	return			hours;
}

u32 XRay::Level::Get_time_minutes()
{
	u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
	split_time((g_pGameLevel && ::Level().game) ? ::Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);
	return			mins;
}

void XRay::Level::Change_game_time(u32 days, u32 hours, u32 mins)
{	
	if (::Level().Server->game && ai().get_alife())
	{
		u32 value = days * 86400 + hours * 3600 + mins * 60;
		float fValue = static_cast<float> (value);
		value *= 1000;//msec		
		Environment().ChangeGameTime(fValue);
		::Level().Server->game->alife().time_manager().change_game_time(value);
	}
}
/*
float XRay::Level::High_cover_in_direction(u32 level_vertex_id, const Fvector &direction)
{
	float			y, p;
	direction.getHP(y, p);
	return			(ai().level_graph().high_cover_in_direction(y, level_vertex_id));
}

float XRay::Level::Low_cover_in_direction(u32 level_vertex_id, const Fvector &direction)
{
	float			y, p;
	direction.getHP(y, p);
	return			(ai().level_graph().low_cover_in_direction(y, level_vertex_id));
}
*/