#include "stdafx.h"
#include "GameTaskManager.h"
#include "alife_registry_wrappers.h"
#include "../xrUICore/xrUIXmlParser.h"
#include "GameTask.h"
#include "Level.h"
#include "map_manager.h"
#include "map_location.h"
#include "actor.h"
#include "UIGame.h"
#include "ui/UIPDAWnd.h"
#include "ui/UIMapWnd.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

shared_str	g_active_task_id;

struct FindTaskByID
{
	shared_str	id;
	bool		b_only_inprocess;
	FindTaskByID(const shared_str& s, bool search_only_inprocess) :id(s), b_only_inprocess(search_only_inprocess) {}
	bool operator () (const SGameTaskKey& key)
	{
		if (b_only_inprocess)
			return (id == key.task_id && key.game_task->GetTaskState() == eTaskStateInProgress);
		else
			return (id == key.task_id);
	}
};

bool task_prio_pred(const SGameTaskKey& k1, const SGameTaskKey& k2)
{
	return k1.game_task->m_priority > k2.game_task->m_priority;
}

CGameTaskManager::CGameTaskManager()
{
	m_gametasks_wrapper			= xr_new<CGameTaskWrapper>();
	m_gametasks_wrapper->registry().init(0);// actor's id
	m_flags.zero				();
	m_flags.set					(eChanged, true);
	m_gametasks					= nullptr;

	if( g_active_task_id.size() )
	{
		CGameTask* t = HasGameTask( g_active_task_id, true );
		if ( t )
		{
			SetActiveTask( t );
		}
	}
}

CGameTaskManager::~CGameTaskManager()
{
	delete_data					(m_gametasks_wrapper);
	g_active_task_id			= nullptr;
}

vGameTasks&	CGameTaskManager::GetGameTasks	() 
{
	if(!m_gametasks)
	{
		m_gametasks = &m_gametasks_wrapper->registry().objects();
#ifdef DEBUG
		Msg("m_gametasks size=%d",m_gametasks->size());
#endif // #ifdef DEBUG
	}

	return *m_gametasks;
}

CGameTask* CGameTaskManager::HasGameTask(const shared_str& id, bool only_inprocess)
{
	FindTaskByID key(id, only_inprocess);
	vGameTasks& gameTasks = GetGameTasks();
    auto it = std::find_if(gameTasks.begin(), gameTasks.end(), key);
	if( it!= gameTasks.end() )
		return (*it).game_task;
	
	return nullptr;
}

CGameTask*	CGameTaskManager::GiveGameTaskToActor(CGameTask* t, u32 timeToComplete, bool bCheckExisting, u32 timer_ttl)
{
	t->CommitScriptHelperContents	();
	if(/* bCheckExisting &&*/ HasGameTask(t->m_ID, true) ) 
	{
 		Msg("! task [%s] already inprocess",t->m_ID.c_str());
		VERIFY_FORMAT( 0, "give_task : Task [%s] already inprocess!", t->m_ID.c_str());
		return nullptr;
	}

	m_flags.set						(eChanged, true);

	GetGameTasks().emplace_back		(t->m_ID );
	GetGameTasks().back().game_task	= t;
	t->m_ReceiveTime				= Level().GetGameTime();
	t->m_TimeToComplete				= t->m_ReceiveTime + timeToComplete * 1000; //ms
	t->m_timer_finish				= t->m_ReceiveTime + timer_ttl      * 1000; //ms

	std::stable_sort				(GetGameTasks().begin(), GetGameTasks().end(), task_prio_pred);

	t->OnArrived					();

	SetActiveTask( t );

	//установить флажок необходимости прочтения тасков в PDA
	if (GameUI())
		GameUI()->UpdatePda();

	t->ChangeStateCallback();

	return t;
}

void CGameTaskManager::SetTaskState(CGameTask* t, ETaskState state)
{
	m_flags.set						(eChanged, true);

	t->SetTaskState					(state);
	
	if ( ActiveTask() == t )
	{
		//SetActiveTask	("");
		g_active_task_id = "";
	}

	if (GameUI())
		GameUI()->UpdatePda();
}

void CGameTaskManager::SetTaskState(const shared_str& id, ETaskState state)
{
	CGameTask* t				= HasGameTask(id, true);
	if (nullptr==t)				{Msg("actor does not has task [%s] or it is completed", *id);	return;}
	SetTaskState				(t, state);
}

void CGameTaskManager::UpdateTasks						()
{
	if(Device.Paused())		return;

	Level().MapManager().DisableAllPointers();

	size_t task_count = GetGameTasks().size();
	if(!task_count)	return;

	{
		using Tasks = buffer_vector<SGameTaskKey>;
		Tasks tasks(_alloca(task_count * sizeof(SGameTaskKey)), task_count, GetGameTasks().begin(), GetGameTasks().end());

		for (SGameTaskKey &it : tasks)
		{
			CGameTask* const	t = it.game_task;
			if (t->GetTaskState() != eTaskStateInProgress)
				continue;

			ETaskState const	state = t->UpdateState();

			if ((state == eTaskStateFail) || (state == eTaskStateCompleted))
				SetTaskState(t, state);
		}
	}

	CGameTask* t = ActiveTask();
	if (t)
	{
		CMapLocation* ml = t->LinkedMapLocation();
		if (ml && !ml->PointerEnabled())
		{
			ml->EnablePointer();
		}
	}

	if(	m_flags.test(eChanged) )
		UpdateActiveTask	();
}


void CGameTaskManager::UpdateActiveTask()
{
	std::stable_sort			(GetGameTasks().begin(), GetGameTasks().end(), task_prio_pred);

	CGameTask*	t			= ActiveTask();
	if ( !t )
	{
		CGameTask* front	= IterateGet(nullptr, eTaskStateInProgress, true);
		if ( front )
		{
			SetActiveTask	(front);
		}
	}

	m_flags.set					(eChanged, false);
	m_actual_frame				= Device.dwFrame;
}

CGameTask* CGameTaskManager::ActiveTask()
{
	const shared_str&	t_id	= g_active_task_id;
	if(!t_id.size())			return nullptr;
	return						HasGameTask( t_id, true );
}

void CGameTaskManager::SetActiveTask(CGameTask* task)
{
	if (task)
	{
		g_active_task_id = task->m_ID;
		m_flags.set(eChanged, true);
		task->m_read = true;
	}
}

CUIMapWnd* GetMapWnd();

void CGameTaskManager::MapLocationRelcase(CMapLocation* ml)
{
	CUIMapWnd* mwnd = GetMapWnd();
	if(mwnd)
		mwnd->MapLocationRelcase(ml);

	CGameTask* gt = HasGameTask(ml, false);
	if(gt)
		gt->RemoveMapLocations(true);
}

CGameTask* CGameTaskManager::HasGameTask(const CMapLocation* ml, bool only_inprocess)
{
    auto it		= GetGameTasks().begin();
    auto it_e		= GetGameTasks().end();

	for(; it!=it_e; ++it)
	{
		CGameTask* gt = (*it).game_task;
		if(gt->LinkedMapLocation()==ml)
		{
			if(only_inprocess && gt->GetTaskState()!=eTaskStateInProgress)
				continue;

			return gt;
		}
	}
	return nullptr;
}

CGameTask* CGameTaskManager::IterateGet(CGameTask* t, ETaskState state, bool bForward)
{
	vGameTasks& v = GetGameTasks();
	size_t cnt = v.size();
	for (size_t i = 0; i < cnt; ++i)
	{
		CGameTask* gt = v[i].game_task;
		if (gt == t || !t)
		{
			bool allow;
			if (bForward)
			{
				if (t) ++i;
				allow = i < cnt;
			}
			else
			{
				allow = (i > 0) && (--i >= 0);
			}
			if (allow)
			{
				CGameTask* found = v[i].game_task;
				if (found->GetTaskState() == state)
					return found;
				else
					return IterateGet(found, state, bForward);
			}
			else return nullptr;
		}
	}
	return nullptr;
}

u32 CGameTaskManager::GetTaskIndex(CGameTask* t, ETaskState state)
{
	if (!t)
	{
		return 0;
	}

	vGameTasks& v = GetGameTasks();
	size_t cnt = v.size();
	u32 res = 0u;
	for (size_t i = 0ul; i < cnt; ++i)
	{
		CGameTask* gt = v[i].game_task;
		if (gt->GetTaskState() == state)
		{
			++res;
			if (gt == t)
			{
				return res;
			}
		}
	}
	return 0u;
}

u32 CGameTaskManager::GetTaskCount(ETaskState state)
{
	vGameTasks& v = GetGameTasks();
	size_t cnt = v.size();
	u32 res = 0u;
	for (size_t i = 0ul; i < cnt; ++i)
	{
		CGameTask* gt = v[i].game_task;
		if (gt->GetTaskState() == state)
		{
			++res;
		}
	}
	return res;
}

char* sTaskStates[]=
{
	"eTaskStateFail",
	"TaskStateInProgress",
	"TaskStateCompleted",
	"TaskStateDummy"
};

void CGameTaskManager::DumpTasks()
{
    auto it			= GetGameTasks().begin();
    auto it_e			= GetGameTasks().end();
	for(; it!=it_e; ++it)
	{
		const CGameTask* gt = (*it).game_task;
		Msg( " ID=[%s] state=[%s] prio=[%d] ",
			gt->m_ID.c_str(),
			sTaskStates[gt->GetTaskState()],
			gt->m_priority );
	}
}