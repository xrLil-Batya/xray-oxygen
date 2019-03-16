////////////////////////////////////////////////////////////////////////////
//	Module 		: autosave_manager.cpp
//	Created 	: 04.11.2004
//  Modified 	: 04.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Autosave manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autosave_manager.h"
#include "../xrEngine/date_time.h"
#include "ai_space.h"
#include "level.h"
#include "xrMessages.h"
#include "UIGame.h"
#include "Actor.h"

extern LPCSTR alife_section;

CAutosaveManager::CAutosaveManager()
{
	u32 hours, minutes, seconds;
	LPCSTR section = alife_section;

	sscanf(pSettings->r_string(section, "autosave_interval"), "%d:%d:%d", &hours, &minutes, &seconds);
	m_autosave_interval = (u32)generate_time(1, 1, 1, hours, minutes, seconds);
	m_last_autosave_time = Device.dwTimeGlobal;

	sscanf(pSettings->r_string(section, "delay_autosave_interval"), "%d:%d:%d", &hours, &minutes, &seconds);
	m_delay_autosave_interval = (u32)generate_time(1, 1, 1, hours, minutes, seconds);

	m_not_ready_count = 0;

	shedule.t_min = 5000;
	shedule.t_max = 5000;
	shedule_register();
}

CAutosaveManager::~CAutosaveManager()
{
	shedule_unregister();
}

float CAutosaveManager::shedule_Scale()
{
	return (.5f);
}

void CAutosaveManager::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}

void CAutosaveManager::on_game_loaded()
{
	m_last_autosave_time = Device.dwTimeGlobal;
}