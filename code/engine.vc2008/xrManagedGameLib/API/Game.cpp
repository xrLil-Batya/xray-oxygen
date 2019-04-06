#include "stdafx.h"
#include "Game.h"
#include "../xrServerEntities/xrServer_Objects_ALife_Monsters.h"
#include "../xrServerEntities/script_storage.h"
#include "../xrEngine/date_time.h"
#include "../xrGame/Level.h"
#include "../xrGame/Actor.h"
#include "../xrGame/ai_space.h"
#include "../xrGame/alife_simulator.h"
#include "../xrGame/alife_time_manager.h"
#include "../xrGame/alife_object_registry.h"
#include "../xrGame/relation_registry.h"
#include "../xrGame/ui/UIGameTutorial.h"
#include "../xrGame/HUDManager.h"
#include "../xrGame/UIGame.h"
#include "../xrGame/ui/UIMainIngameWnd.h"
#include "../xrGame/ui/UIMotionIcon.h"

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

extern GAME_API CUISequencer* g_tutorial;
extern GAME_API CUISequencer* g_tutorial2;

void XRay::Game::setTutorialState(LPCSTR name, eTutorialState tutorialState)
{
	switch (tutorialState)
	{
		case eTutorialState::eStart : 
		{
			if (g_tutorial) {
				VERIFY(!g_tutorial2);
				g_tutorial2 = g_tutorial;
			};

			g_tutorial = xr_new<CUISequencer>();
			g_tutorial->Start(name);
			if (g_tutorial2)
				g_tutorial->m_pStoredInputReceiver = g_tutorial2->m_pStoredInputReceiver;

		}break;

		case eTutorialState::eStop :
		{
			if (g_tutorial)
				g_tutorial->Stop();
		}break;
	}

}
bool XRay::Game::TutorialState::get()
{
	return (g_tutorial != nullptr);
}

void XRay::Game::GlobalSend(NET_Packet& P)
{
	::Level().Send(P);
}

void XRay::Game::UserEventGen(NET_Packet& P, u32 _event, u32 _dest)
{
	CGameObject::u_EventGen(P, _event, _dest);
}

::System::String^ XRay::Game::TranslateString(LPCSTR str)
{
	return gcnew ::System::String(str);
}
Fbox XRay::Game::GetBoundingVolume()
{
	return ::Level().ObjectSpace.GetBoundingVolume();
}
void XRay::Game::IterateSounds(LPCSTR prefix, u32 max_count, CallBack callback)
{
	for (int j = 0, N = _GetItemCount(prefix); j < N; ++j) {
		string_path					fn, s;
		LPSTR						S = (LPSTR)&s;
		_GetItem(prefix, j, s);
		if (FS.exist(fn, "$game_sounds$", S, ".ogg"))
			callback(gcnew ::System::String(prefix));

		for (u32 i = 0; i < max_count; ++i)
		{
			string_path					name;
			xr_sprintf(name, "%s%d", S, i);
			if (FS.exist(fn, "$game_sounds$", name, ".ogg"))
				callback(gcnew ::System::String(name));
		}
	}
}
void XRay::Game::PrefetchSnd(LPCSTR name)
{
	(::Level().PrefetchSound(name));
}

XRay::GameObject^ XRay::Game::GlobalTargetObject()
{
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	if (RQ.O)
	{
		CGameObject	*game_object = static_cast<CGameObject*>(RQ.O);
		if (game_object)
			return gcnew GameObject(::System::IntPtr(game_object));
	}
	return (nullptr);
}

float XRay::Game::GlobalTargetDist()
{
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	if (RQ.range)
		return RQ.range;
	return (0);
}

void XRay::Game::SpawnSection(LPCSTR sSection, ::System::Numerics::Vector3 vPosition, u32 LevelVertexID, u16 ParentID, bool bReturnItem)
{
	Fvector NativeVect; NativeVect.set(vPosition.X, vPosition.Y, vPosition.Z);
	::Level().spawn_item(sSection, NativeVect, LevelVertexID, ParentID, bReturnItem);
}

::System::UInt32 XRay::Game::GlobalTargetElement()
{
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	if (RQ.element)
	{
		return RQ.element;
	}
	return (0);
}