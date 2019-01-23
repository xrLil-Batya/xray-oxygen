////////////////////////////////////////////////////////////////////////////
//	Module 		: profiler.cpp
//	Created 	: 23.07.2004
//  Modified 	: 10.01.2019
//	Author		: Dmitriy Iassenev
//	Description : Profiler
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "profiler.h"
#include "../xrEngine/gamefont.h"

CProfiler ENGINE_API	*g_profiler			= nullptr;

CProfiler::CProfiler				()
	: m_portionSize(0)
{
	m_portionAllocated = 10000;
	m_portions.resize(m_portionAllocated);

	m_schedulePortions.reserve(1000);
}

void RecursiveDeleteProfiles(CProfileStats* profile)
{
	for (auto& ChildrenPair : profile->childrens)
	{
		RecursiveDeleteProfiles(ChildrenPair.second);
	}

	profile->childrens.clear();

	delete profile;
}

CProfiler::~CProfiler				()
{
	for (auto& ProfilePair : m_rootProfiles)
	{
		RecursiveDeleteProfiles(ProfilePair.second);
	}

	m_rootProfiles.clear();
	m_profiles.clear();
}

void CProfiler::clear ()
{
	xrCriticalSectionGuard guard(m_section);

	ZeroMemory(m_portions.data(), sizeof(CProfileResultPortion) * m_portionSize);
	m_portionSize = 0;

	m_schedulePortions.clear();
	m_schedulePortions.reserve(1000);
}

void CProfiler::show_stats(CGameFont *game_font)
{
	for (CProfileResultPortion& profilePortion : m_portions)
	{
		// check if we meet null record
		if (profilePortion.m_timer_id == nullptr) break;

		// Try get profile by his full name
		CProfileStats* profile = nullptr;
		auto profileIter = m_profiles.find(profilePortion.m_timer_id);
		if (profileIter == m_profiles.end())
		{
			// Create profile keycard
			xr_string profileName(profilePortion.m_timer_id);
			xr_vector<xr_string> nameTokens = profileName.Split('/');
			R_ASSERT(!nameTokens.empty());

			if (nameTokens.size() == 1)
			{
				// root
				profile = new CProfileStats(shared_str(nameTokens[0].c_str()));
				m_rootProfiles.insert(std::make_pair(profile->name, profile));
			}
			else
			{
				// create a parent structure, if not existed
				// example: "ALife/NPC/Vanya" will check for "Alife" and "NPC" profiles and create them
				// if they are not exist. 
				CProfileStats* CurrentRoot = nullptr;
				for (int i = 0; i < nameTokens.size(); ++i)
				{
					shared_str SubName(nameTokens[i].c_str());
					CurrentRoot = GetOrCreateProfile(SubName, CurrentRoot);
				}
				profile = CurrentRoot;
			}

			m_profiles.insert(std::make_pair(profilePortion.m_timer_id, profile));
		}
		else
		{
			profile = profileIter->second;
		}

		UpdateProfileByTime(profile, profilePortion.m_time);
	}

	if (!!psDeviceFlags.test(rsGameProfiler))
	{
		ZeroMemory(m_portions.data(), sizeof(CProfileResultPortion) * m_portionSize);
		m_portionSize = 0;

		if (m_portions.size() > 10000)
		{
			// try lowering memory
			m_portionAllocated = 10000;
			m_portions.resize(m_portionAllocated);
		}

		u32 MaxNameLenght = 0;
		for (auto& ChildProfilePair : m_rootProfiles)
		{
			MaxNameLenght = std::max(MaxNameLenght, ChildProfilePair.second->name.size());
		}

		// show time!
		for (auto& rootProfilePair : m_rootProfiles)
		{
			ShowProfileStat(game_font, *rootProfilePair.second, nullptr, 0, MaxNameLenght);
		}
	}

	if (!!psDeviceFlags.test(rsScheduleProfiler))
	{
		for (const CProfileScheduledPortion& profilePortion : m_schedulePortions)
		{
			CProfileStats* profile = nullptr;
			auto scheduleProfileIter = m_scheduleProfiles.find(profilePortion.m_timer_id);
			if (scheduleProfileIter == m_scheduleProfiles.end())
			{
				auto InsertedIter = m_scheduleProfiles.insert(std::make_pair(profilePortion.m_timer_id, CProfileStats(profilePortion.m_timer_id)));
				profile = &InsertedIter.first->second;
			}
			else
			{
				profile = &scheduleProfileIter->second;
			}

			UpdateProfileByTime(profile, profilePortion.m_time);
		}

		m_schedulePortions.clear();
		m_schedulePortions.reserve(1000);

		u32 MaxNameLenght = 0;
		for (auto& scheduleProfilePair : m_scheduleProfiles)
		{
			MaxNameLenght = std::max(MaxNameLenght, scheduleProfilePair.second.name.size());
		}

		// show time
		for (auto& scheduleProfilePair : m_scheduleProfiles)
		{
			CProfileStats& profile = scheduleProfilePair.second;

			ShowProfileStat(game_font, profile, nullptr, 0, MaxNameLenght);
		}
	}
}

void CProfiler::UpdateProfileByTime(CProfileStats* profile, u64 time)
{
	R_ASSERT(profile != nullptr);
	profile->count++;

	float fTime = 1000.f * float(double(time) / double(CPU::qpc_freq));

	profile->time += fTime;

	if (profile->min_time > fTime)
	{
		profile->min_time = fTime;
	}

	if (profile->max_time < fTime)
	{
		profile->max_time = fTime;
	}

	profile->total_time += fTime;
}

void CProfiler::add_profile_portion	(const CProfileResultPortion &profile_portion)
{
	xrCriticalSectionGuard guard(m_section);
	m_portions[m_portionSize++] = profile_portion;

	if (m_portionAllocated == m_portionSize)
	{
		m_portionAllocated += 10000;
		m_portions.resize(m_portionAllocated);
	}
}

void CProfiler::add_profile_portion(const CProfileSchedule &profile_portion)
{
	// no possible race condition here, no need to lock m_section
	m_schedulePortions.push_back(profile_portion);
}

CProfileStats* CProfiler::GetOrCreateProfile(shared_str InName, CProfileStats* InParent /*= nullptr*/)
{
	xr_unordered_map<shared_str, CProfileStats*>* pSearchVec = &m_rootProfiles;
	if (InParent != nullptr)
	{
		pSearchVec = &InParent->childrens;
	}

	auto existedProfileIter = pSearchVec->find(InName);
	if (existedProfileIter != pSearchVec->end())
	{
		return existedProfileIter->second;
	}

	//auto ProfileIter = m_profiles.insert(std::make_pair(InName, CProfileStats(InName)));
	CProfileStats* pProfile = new CProfileStats(InName);
	if (InParent == nullptr)
	{
		m_rootProfiles.insert(std::make_pair(pProfile->name, pProfile));
	}
	else
	{
		InParent->childrens.insert(std::make_pair(pProfile->name, pProfile));
	}
	return pProfile;
}

void CProfiler::ShowProfileStat(CGameFont* game_font, CProfileStats& Stat, const CProfileStats* Parent, int Level, u32 MaxNameLenght)
{
	string128 Tabs;
	Tabs[0] = '\0';
	memset(Tabs, ' ', 2 * Level);
	Tabs[Level * 2 + 1] = '\0';

	int SpacesRequired = int(MaxNameLenght - Stat.name.size());
	SpacesRequired = std::max(SpacesRequired, 0);
	string128 AlignSpaces;
	AlignSpaces[0] = '\0';
	memset(AlignSpaces, ' ', SpacesRequired);
	AlignSpaces[SpacesRequired] = '\0';

	if (Stat.time < Stat.previous_time)
	{
		Stat.time = 0.99f * Stat.previous_time + 0.01f * Stat.time;
	}

	if (Stat.min_time < 9995.0f) // 9999.0f is a marker that profile was not update even a single time (created as parent)
	{
// 		if (Parent != nullptr)
// 		{
// 			game_font->OutNext("%s%s: %2.2fms, %2.1f%% (min: %2.2fms, max: %2.2fms) [%u]",
// 				Tabs, Stat->name.c_str(), Stat->time, 100.0f * float(Stat->time) / float(Parent->time), Stat->min_time, Stat->max_time, Stat->count);
// 		}
// 		else
// 		{
// 			game_font->OutNext("%s%s: %2.2fms, (min: %2.2fms, max: %2.2fms) [%u]",
// 				Tabs, Stat->name.c_str(), Stat->time, Stat->min_time, Stat->max_time, Stat->count);
// 		}

		game_font->OutNext("%s%s%s: %2.2fms, (min: %2.2fms, max: %2.2fms, total: %2.2fms) [%u]",
			Tabs, Stat.name.c_str(), AlignSpaces, Stat.time, Stat.min_time, Stat.max_time, Stat.total_time, Stat.count);
	}
	else
	{
		game_font->OutNext("%s%s%s: [%u]",
			Tabs, Stat.name.c_str(), AlignSpaces, Stat.count);
	}

	u32 ChildMaxNameLenght = 0;
	for (auto& ChildProfilePair : Stat.childrens)
	{
		ChildMaxNameLenght = std::max(ChildMaxNameLenght, ChildProfilePair.second->name.size());
	}

	for (auto& ChildProfilePair : Stat.childrens)
	{
		ShowProfileStat(game_font, *ChildProfilePair.second, &Stat, Level + 1, ChildMaxNameLenght);
	}

	Stat.count = 0;
	Stat.previous_time = Stat.time;
	Stat.time = 0.0f;
}

CProfileStats::CProfileStats(const CProfileResultPortion& portion)
{
	xr_string profileName(portion.m_timer_id);
	xr_vector<xr_string> nameTokens = profileName.Split('/');
	R_ASSERT(!nameTokens.empty());

	// get last token as name
	name = nameTokens[nameTokens.size() - 1].c_str();
}

CProfileStats::CProfileStats(shared_str portionName)
	: name (portionName)
{}


CProfileSchedule::CProfileSchedule(shared_str timer_id)
{
	if (!psDeviceFlags.test(rsScheduleProfiler))
		return;

	if (!psDeviceFlags.test(rsStatistic))
		return;

	m_timer_id = timer_id;
	m_time = CPU::QPC();
}

CProfileSchedule::~CProfileSchedule()
{
	if (!psDeviceFlags.test(rsScheduleProfiler))
		return;

	if (!psDeviceFlags.test(rsStatistic))
		return;

	u64 temp = CPU::QPC();
	m_time = temp - m_time;
	profiler().add_profile_portion(*this);
}

CProfilePortion::CProfilePortion(LPCSTR timer_id)
{
	if (!psDeviceFlags.test(rsGameProfiler))
		return;

	if (!psDeviceFlags.test(rsStatistic))
		return;

	m_timer_id = timer_id;
	m_time = CPU::QPC();
}

CProfilePortion::~CProfilePortion()
{
	if (!psDeviceFlags.test(rsGameProfiler))
		return;

	if (!psDeviceFlags.test(rsStatistic))
		return;

	u64 temp = CPU::QPC();
	m_time = temp - m_time;
	profiler().add_profile_portion(*this);
}
