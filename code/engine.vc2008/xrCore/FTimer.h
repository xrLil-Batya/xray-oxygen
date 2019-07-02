#pragma once

class CTimerBase
{
protected:
    u64 qwStartTime;
    u64 qwPausedTime;
    u64 qwPauseAccum;
    bool bPause;

public:
	constexpr CTimerBase() : qwStartTime(0), qwPausedTime(0), qwPauseAccum(0), bPause(false) { }

    inline void Start() 
    { 
        if (bPause) 
            return; 
        qwStartTime = CPU::QPC() - qwPauseAccum; 
    }

    inline u64		GetElapsed_ticks() const { return bPause ? qwPausedTime : CPU::QPC() - qwStartTime - qwPauseAccum; }
    inline u32		GetElapsed_ms	() const { return u32(GetElapsed_ticks() * 1000 / CPU::qpc_freq); }
    inline float	GetElapsed_sec	() const { return float(double(GetElapsed_ticks()) / double(CPU::qpc_freq)); }
	inline void		Dump			() const { Msg("* Elapsed time (sec): %f", GetElapsed_sec()); }
};

class CTimer : public CTimerBase
{
    using inherited = CTimerBase;

    float	m_time_factor;
    u64		m_real_ticks;
    u64		m_ticks;

private:
    inline u64 GetElapsed_ticks(const u64& current_ticks) const
    {
        u64 delta = current_ticks - m_real_ticks;
        double delta_d = (double)delta;
        double time_factor_d = time_factor();
        double time = delta_d*time_factor_d + .5;
        u64 result = (u64)time;
        return (m_ticks + result);
    }

public:
    constexpr CTimer() : m_time_factor(1.f), m_real_ticks(0), m_ticks(0) {}

	inline const float& time_factor		() const { return (m_time_factor); }
	inline u64			GetElapsed_ticks() const { return GetElapsed_ticks(inherited::GetElapsed_ticks()); }
    inline u32			GetElapsed_ms	() const { return (u32(GetElapsed_ticks()*u64(1000) / CPU::qpc_freq)); }
    inline float		GetElapsed_sec	() const { return float(double(GetElapsed_ticks()) / double(CPU::qpc_freq)); }
    inline void			Dump			() const { Msg("* Elapsed time (sec): %f", GetElapsed_sec()); }

	inline void Start()
	{
		if (bPause) return;

		inherited::Start();

		m_real_ticks = 0;
		m_ticks = 0;
	}

	inline void time_factor(const float& time_factor)
	{
		u64 current = inherited::GetElapsed_ticks();
		m_ticks = GetElapsed_ticks(current);
		m_real_ticks = current;
		m_time_factor = time_factor;
	}
};

class CTimer_paused_ex : public CTimer
{
    u64 save_clock;
public:
				 CTimer_paused_ex	() = default;
	virtual		 ~CTimer_paused_ex	() = default;
    inline	bool Paused				() const { return bPause; }

    inline	void Pause(bool b)
    {
        if (bPause == b) return;
		bPause = b;

        const u64 _current = CPU::QPC();
        if (bPause)
        {
            save_clock = _current;
            qwPausedTime = CTimerBase::GetElapsed_ticks();
        }
        else
        {
            qwPauseAccum += _current - save_clock;
        }
    }
};

class CTimer_paused;

class pauseMngr 
{
    xr_vector<CTimer_paused_ex*> m_timers;
    bool paused;
public:
    pauseMngr() :paused(false)
    {
        m_timers.reserve(3);
    }
	inline bool Paused	() const			{ return paused; }
	inline void Register(CTimer_paused* t)	{ m_timers.push_back(reinterpret_cast<CTimer_paused_ex*> (t)); }

	inline void Pause (const bool b)
    {
        if (paused == b) return;

        for (CTimer_paused_ex* pPaused: m_timers)
			pPaused->Pause(b);

        paused = b;
    }

	inline void UnRegister(CTimer_paused* t)
    {
        CTimer_paused_ex* timer = reinterpret_cast<CTimer_paused_ex*>(t);
        xr_vector<CTimer_paused_ex*>::iterator it = std::find(m_timers.begin(), m_timers.end(), timer);
        if (it != m_timers.end())
            m_timers.erase(it);
    }
};

extern XRCORE_API pauseMngr g_pauseMngr;
extern XRCORE_API bool g_bEnableStatGather;

class CTimer_paused : public CTimer_paused_ex 
{
public:
    inline CTimer_paused() { g_pauseMngr.Register(this); }
    inline virtual ~CTimer_paused() { g_pauseMngr.UnRegister(this); }
};

struct XRCORE_API CStatTimer
{
    CTimer	T;
	u32		count;
    u64		accum;
    double	result;

	constexpr	 CStatTimer() : result(0.0), accum(0), count(0) {}
    void		 FrameStart();
    void		 FrameEnd  ();
				 
    inline void	 Begin	() { if (!g_bEnableStatGather) return; count++; T.Start(); }
    inline void	 End	() { if (!g_bEnableStatGather) return; accum += T.GetElapsed_ticks(); }

	inline u64	 GetElapsed_ticks	() const { return accum; }

	inline u32	 GetElapsed_ms		() const { return u32(GetElapsed_ticks()*u64(1000) / CPU::qpc_freq); }
	inline float GetElapsed_sec		() const { return float(double(GetElapsed_ticks()) / double(CPU::qpc_freq)); }
};

class XRCORE_API ScopeStatTimer
{
	CStatTimer& _timer;
public:
	ScopeStatTimer(CStatTimer& destTimer);
	~ScopeStatTimer();
};