#include "stdafx.h"
#include "eventapi.h"
#include "xr_ioconsole.h"
#include <ppl.h>

class ENGINE_API CEvent
{
	friend class CEventAPI;
private:
	char*						Name;
	xr_vector<IEventReceiver*>		Handlers;
	u32							dwRefCount;
public:
	CEvent(const char* S);
	~CEvent();

	LPCSTR	GetFull()
	{
		return Name;
	}
	u32	RefCount()
	{
		return dwRefCount;
	}

	BOOL	Equal(CEvent& E)
	{
		return stricmp(Name, E.Name) == 0;
	}

	void	Attach(IEventReceiver* H)
	{
		if (std::find(Handlers.begin(), Handlers.end(), H) == Handlers.end())
			Handlers.push_back(H);
	}
	void	Detach(IEventReceiver* H)
	{
		xr_vector<IEventReceiver*>::iterator I = std::find(Handlers.begin(), Handlers.end(), H);
		if (I != Handlers.end())
			Handlers.erase(I);
	}
	void	Signal(u64 P1, u64 P2)
	{
		for (IEventReceiver* iter: Handlers)
			iter->OnEvent(this, P1, P2);
	}
};
//-----------------------------------------
CEvent::CEvent(const char* S)
{
	Name = xr_strdup(S);
	_strupr(Name);
	dwRefCount = 1;
}

CEvent::~CEvent()
{
	xr_free(Name);
}

//-----------------------------------------
IC bool ev_sort(CEvent*E1, CEvent*E2)
{
	return E1->GetFull() < E2->GetFull();
}

void CEventAPI::Dump()
{
	std::sort(Events.begin(), Events.end(), ev_sort);
	for (EVENT &it: Events)
		Msg("* [%d] %s", it->RefCount(), it->GetFull());
}

EVENT CEventAPI::Create(const char* N)
{
	xrCriticalSectionGuard guard(CS);
	CEvent E(N);

	for (EVENT &F: Events)
	{
		if (F->Equal(E))
		{
			F->dwRefCount++;
			return F;
		}
	}

	EVENT X = xr_new<CEvent>(N);
	Events.push_back(X);
	return X;
}

void CEventAPI::Destroy(EVENT& E)
{
	xrCriticalSectionGuard guard(CS);
	E->dwRefCount--;
	if (E->dwRefCount == 0)
	{
		xr_vector<CEvent*>::iterator I = std::find(Events.begin(), Events.end(), E);
		R_ASSERT(I != Events.end());
		Events.erase(I);
		xr_delete(E);
	}
}

EVENT	CEventAPI::Handler_Attach(const char* N, IEventReceiver* H)
{
	xrCriticalSectionGuard guard(CS);
	EVENT	E = Create(N);
	E->Attach(H);
	return E;
}

void CEventAPI::Handler_Detach(EVENT& E, IEventReceiver* H)
{
	if (E == nullptr)	return;
	xrCriticalSectionGuard guard(CS);
	E->Detach(H);
	Destroy(E);
}

void CEventAPI::Signal(EVENT E, u64 P1, u64 P2)
{
	xrCriticalSectionGuard guard(CS);
	E->Signal(P1, P2);
}

void CEventAPI::Signal(LPCSTR N, u64 P1, u64 P2)
{
	xrCriticalSectionGuard guard(CS);
	EVENT		E = Create(N);
	Signal(E, P1, P2);
	Destroy(E);
}
void CEventAPI::Defer(EVENT E, u64 P1, u64 P2)
{
	xrCriticalSectionGuard guard(CS);
	E->dwRefCount++;
	Events_Deferred.emplace_back();
	Events_Deferred.back().E = E;
	Events_Deferred.back().P1 = P1;
	Events_Deferred.back().P2 = P2;
}

void CEventAPI::Defer(LPCSTR N, u64 P1, u64 P2)
{
	xrCriticalSectionGuard guard(CS);
	EVENT	E = Create(N);
	Defer(E, P1, P2);
	Destroy(E);
}

void CEventAPI::OnFrame()
{
	xrCriticalSectionGuard guard(CS);
	if (Events_Deferred.empty())	return;
	for (CEventAPI::Deferred& DEF : Events_Deferred)
	{
		Signal(DEF.E, DEF.P1, DEF.P2);
		Destroy(DEF.E);
	}
	Events_Deferred.clear();
}

BOOL CEventAPI::Peek(LPCSTR EName)
{
	xrCriticalSectionGuard guard(CS);
	if (Events_Deferred.empty()) return FALSE;
	for (Deferred&	DEF : Events_Deferred)
	{
		if (_stricmp(DEF.E->GetFull(), EName) == 0) return TRUE;
	}

	return FALSE;
}

void CEventAPI::_destroy()
{
	Dump();
	if (Events.empty())				Events.clear();
	if (Events_Deferred.empty())	Events_Deferred.clear();
}
