#include "stdafx.h"
#include "Level.h"
#include "../xrCore/LocatorAPI.h"
#include "../xrGame/patrol_path_storage.h"
#include "../xrGame/ai_space.h"
#include "../xrGame/alife_simulator.h"
#include "../xrGame/alife_time_manager.h"
#include "../xrGame/level_graph.h"
#include "../xrGame/Level.h"
#include "../xrGame/Actor.h"
#include "../xrEngine/date_time.h"
#include "../xrGame/map_location.h"
#include "../xrGame/map_manager.h"



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

void XRay::Level::WeatherFX::set(::System::String^ str)
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

bool XRay::Level::iSWfxPlaying()
{
	return (::Environment().IsWeatherFXPlaying());
}

float XRay::Level::WfxTime::get()
{
	return (::Environment().wfx_time);
}

void XRay::Level::StopWeatherFX()
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

float XRay::Level::RainFactor::get()
{
	return (::Environment().CurrentEnv->rain_density);
}

u32	XRay::Level::VertexInDirection(u32 level_vertex_id, Fvector direction, float max_distance)
{
	direction.normalize_safe();
	direction.mul(max_distance);
	Fvector			start_position = ai().level_graph().vertex_position(level_vertex_id);
	Fvector			finish_position = Fvector(start_position).add(direction);
	u32				result = u32(-1);
	ai().level_graph().farthest_vertex_in_direction(level_vertex_id, start_position, finish_position, result, nullptr);
	return			(ai().level_graph().valid_vertex_id(result) ? result : level_vertex_id);
}

Fvector XRay::Level::VertexPosition(u32 level_vertex_id)
{
	return			(ai().level_graph().vertex_position(level_vertex_id));
}

float XRay::Level::HighCoverInDirection(u32 level_vertex_id, const Fvector &direction)
{
	float			y, p;
	direction.getHP(y, p);
	return			(ai().level_graph().high_cover_in_direction(y, level_vertex_id));
}

float XRay::Level::LowCoverInDirection(u32 level_vertex_id, const Fvector &direction)
{
	float			y, p;
	direction.getHP(y, p);
	return			(ai().level_graph().low_cover_in_direction(y, level_vertex_id));
}

bool XRay::Level::ValidVertex(u32 level_vertex_id)
{
	return ai().level_graph().valid_vertex_id(level_vertex_id);
}

void XRay::Level::MapAddObjectSpot(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().AddMapLocation(spot_type, id));
	if (xr_strlen(text))
	{
		ml->SetHint(text);
	}
}

void XRay::Level::MapAddObjectSpotSer(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().AddMapLocation(spot_type, id));
	if (xr_strlen(text))
		ml->SetHint(text);

	ml->SetSerializable(true);
}

void XRay::Level::MapChangeSpotHint(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = (::Level().MapManager().GetMapLocation(spot_type, id));
	if (!ml)				return;
	ml->SetHint(text);
}

void XRay::Level::MapRemoveObjectSpot(u16 id, LPCSTR spot_type)
{
	(::Level().MapManager().RemoveMapLocation(spot_type, id));
}

u16 XRay::Level::MapHasObjectSpot(u16 id, LPCSTR spot_type)
{
	return (::Level().MapManager().HasMapLocation(spot_type, id));
}

bool XRay::Level::PatrolPathExists(LPCSTR patrol_path)
{
	return		(!!ai().patrol_paths().path(patrol_path, true));
}

LPCSTR XRay::Level::Name::get()
{
	return		*(::Level().name());
}

void XRay::Level::PrefetchSnd(LPCSTR name)
{
	(::Level().PrefetchSound(name));
}
// CClientSpawnManager нет в Managed, нужно писать ему класс
XRay::ClientSpawnManager^ XRay::Level::ClientSpawnMngr::get()
{
	return gcnew ClientSpawnManager(&(::Level().client_spawn_manager()));
}