////////////////////////////////////////////////////////////////////////////
//	Module 		: profiler.h
//	Created 	: 23.07.2004
//  Modified 	: 10.01.2019
//	Author		: Dmitriy Iassenev
//	Description : Profiler
//  Commentary  : Giperion: Дима, мне кажется ты перемудрил с профайлером
//  Commentary_Later: Giperion: А теперь и я перемудрил -_-
////////////////////////////////////////////////////////////////////////////

#pragma once

struct CProfileResultPortion {
	u64				m_time;
	LPCSTR			m_timer_id;
};

// I don't want create another field in CProfileResultPortion, it will slow down most of statistic
struct CProfileScheduledPortion {
	u64				m_time;
	shared_str		m_timer_id;
};

struct ENGINE_API CProfilePortion : public CProfileResultPortion {
	CProfilePortion		(LPCSTR timer_id);
	~CProfilePortion	();
};

struct CProfileSchedule : public CProfileScheduledPortion {
	CProfileSchedule(shared_str timer_id);
	~CProfileSchedule();
};

struct CProfileStats {
	float			time		  = 0.0f;
	float			previous_time = 0.0f;
	float			min_time	  = 9999.0f;
	float			max_time	  = 0.0f;
	float			total_time	  = 0.0f;
	u32				count		  = 0;
	shared_str		name;
	xr_unordered_map<shared_str, CProfileStats*> childrens;

	CProfileStats(const CProfileResultPortion& portion);
	CProfileStats(shared_str portionName);
};

class ENGINE_API CProfiler
{
public:
	CProfiler			();
	~CProfiler			();
	void		show_stats			(CGameFont *game_font);

	void		clear();
	void		add_profile_portion(const CProfileResultPortion &profile_portion);
	void		add_profile_portion(const CProfileSchedule &profile_portion);

protected:
	CProfileStats* GetOrCreateProfile(shared_str InName, CProfileStats* InParent = nullptr);
	void ShowProfileStat(CGameFont* game_font, CProfileStats& Stat, const CProfileStats* Parent, int Level, u32 MaxNameLenght);
	void UpdateProfileByTime(CProfileStats* profile, u64 time);

	xr_vector<CProfileResultPortion>			m_portions;
	u32	m_portionSize;
	u32	m_portionAllocated;

	xr_vector<CProfileScheduledPortion>			m_schedulePortions;

	xr_unordered_map<shared_str, CProfileStats*> m_profiles;
	xr_unordered_map<shared_str, CProfileStats*> m_rootProfiles;

	// to avoid name collisions, schedule stats profile saved in different container
	xr_unordered_map<shared_str, CProfileStats> m_scheduleProfiles;

	xrCriticalSection	m_section;
};

extern ENGINE_API  CProfiler *g_profiler;

IC	CProfiler&	profiler()
{
	return			(*g_profiler);
}
		
#	define START_PROFILE(a) { CProfilePortion	__profile_portion__(a);
#	define STOP_PROFILE     }
