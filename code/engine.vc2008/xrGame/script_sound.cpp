////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_sound.h"
#include "script_game_object.h"

#include "gameobject.h"
#include "ai_space.h"
#include "script_engine.h"

CScriptSound::CScriptSound(LPCSTR caSoundName, ESoundTypes sound_type)
{
	m_caSoundToPlay = caSoundName;
	string_path l_caFileName;
	VERIFY(::Sound);
	if (FS.exist(l_caFileName,"$game_sounds$",caSoundName,".ogg"))
		m_sound.create(caSoundName,st_Effect,sound_type);
	else
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"File not found \"%s\"!",l_caFileName);
}

CScriptSound::~CScriptSound()
{
	THROW3(!m_sound._feedback(),"playing sound is not completed, but is destroying",m_sound._handle() ? m_sound._handle()->file_name() : "unknown");
	m_sound.destroy();
}

Fvector CScriptSound::GetPosition() const
{
	if (m_sound._handle() != 0)
	{
		const CSound_params	*l_tpSoundParams = m_sound.get_params();
		if (l_tpSoundParams)
			return (l_tpSoundParams->position);
	}
	ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"Sound was not launched, can't get position!");
	return (Fvector().set(0,0,0));
}

void CScriptSound::Play(CScriptGameObject *object, float delay, int flags)
{
	THROW3(m_sound._handle(), "There is no sound", *m_caSoundToPlay);
	m_sound.play((object) ? &object->object() : NULL, flags, delay);
}

void CScriptSound::PlayAtPos(CScriptGameObject *object, const Fvector &position, float delay, int flags)
{
	THROW3(m_sound._handle(),"There is no sound",*m_caSoundToPlay);
	m_sound.play_at_pos((object) ? &object->object() : NULL, position,flags,delay);
}

void CScriptSound::PlayNoFeedback	(CScriptGameObject *object,	u32 flags/*!< Looping */, float delay/*!< Delay */, Fvector pos, float vol)
{
	THROW3(m_sound._handle(),"There is no sound",*m_caSoundToPlay);
	m_sound.play_no_feedback((object) ? &object->object() : NULL, flags,delay,&pos,&vol);
}

#include <luabind/luabind.hpp>
using namespace luabind;

#pragma optimize("gyts",on)
void CScriptSound::script_register(lua_State *L)
{
	module(L)
	[
		class_<CSound_params>("sound_params")
			.def_readwrite("position",		&CSound_params::position)
			.def_readwrite("volume",		&CSound_params::volume)
			.def_readwrite("frequency",		&CSound_params::freq)
			.def_readwrite("min_distance",	&CSound_params::min_distance)
			.def_readwrite("max_distance",	&CSound_params::max_distance),
			
		class_<CScriptSound>("sound_object")
			.enum_("sound_play_type")
			[
				value("looped",	sm_Looped),
				value("s2d",	sm_2D),
				value("s3d",	0)
			]
			.property("frequency",		&CScriptSound::GetFrequency,	&CScriptSound::SetFrequency)
			.property("min_distance",	&CScriptSound::GetMinDistance,	&CScriptSound::SetMinDistance)
			.property("max_distance",	&CScriptSound::GetMaxDistance,	&CScriptSound::SetMaxDistance)
			.property("volume",			&CScriptSound::GetVolume,		&CScriptSound::SetVolume)
			.def(						constructor<LPCSTR>())
			.def(						constructor<LPCSTR,ESoundTypes>())
			.def("get_position",		&CScriptSound::GetPosition)
			.def("set_position",		&CScriptSound::SetPosition)
			.def("play",				(void (CScriptSound::*)(CScriptGameObject*))(&CScriptSound::Play))
			.def("play",				(void (CScriptSound::*)(CScriptGameObject*,float))(&CScriptSound::Play))
			.def("play",				(void (CScriptSound::*)(CScriptGameObject*,float,int))(&CScriptSound::Play))
			.def("play_at_pos",			(void (CScriptSound::*)(CScriptGameObject*,const Fvector &))(&CScriptSound::PlayAtPos))
			.def("play_at_pos",			(void (CScriptSound::*)(CScriptGameObject*,const Fvector &,float))(&CScriptSound::PlayAtPos))
			.def("play_at_pos",			(void (CScriptSound::*)(CScriptGameObject*,const Fvector &,float, int))(&CScriptSound::PlayAtPos))
			.def("play_no_feedback",	&CScriptSound::PlayNoFeedback)
			.def("stop",				&CScriptSound::Stop)
			.def("stop_deffered",		&CScriptSound::StopDeffered)
			.def("playing",				&CScriptSound::IsPlaying)
			.def("length",				&CScriptSound::Length)
			.def("attach_tail",			&CScriptSound::AttachTail)
	];
}
