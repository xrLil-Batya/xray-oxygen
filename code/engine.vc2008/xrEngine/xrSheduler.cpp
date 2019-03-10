/*/////////////////////////////////
// Desc: Async Sheduler Manager	 //
// Author: DjYar				 //
// Modifer: ForserX				 //
*//////////////////////////////////

#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"
#include "profiler.h"

BOOL g_bSheduleInProgress = FALSE;

volatile bool mtShedulerIsWorking = false;
void CSheduler::mtShedulerThread(void* pSheduler)
{
	CSheduler* pTrySheduler = ((CSheduler*)(pSheduler));
	while (true)
	{
		if (!pTrySheduler) break;

		// Initialize
		if (!mtShedulerIsWorking) continue;

		pTrySheduler->internal_Registration();
		g_bSheduleInProgress = TRUE;

		// Realtime priority
		pTrySheduler->m_processing_now = true;
		u32	dwTime = Device.dwTimeGlobal;

		for (CSheduler::Item& item : pTrySheduler->ItemsRT)
		{
			R_ASSERT(item.Object);

#ifdef DEBUG
			VERIFY(item.Object->dbg_startframe != Device.dwFrame);
			item.Object->dbg_startframe = Device.dwFrame;
#endif

			if (!item.Object->shedule_Needed())
			{
				item.dwTimeOfLastExecute = dwTime;
				continue;
			}

			u32	Elapsed = dwTime - item.dwTimeOfLastExecute;

			item.Object->shedule_Update(Elapsed);
			item.dwTimeOfLastExecute = dwTime;

			pTrySheduler->ProcessStep();

			// Finalize
			pTrySheduler->m_processing_now = false;
			g_bSheduleInProgress = FALSE;

			pTrySheduler->internal_Registration();
			mtShedulerIsWorking = false;
		}
	

	}
	
}
//-------------------------------------------------------------------------------------
void CSheduler::Initialize()
{
	m_current_step_obj = nullptr;
	m_processing_now = false;

	//thread_spawn(mtShedulerThread, "X-Ray: Sheduler Process Steper", 0, this);
}

void CSheduler::Destroy()
{
	internal_Registration();

	ItemsRT.clear();
	Items.clear();
	ItemsProcessed.clear();
	Registration.clear();
}

void CSheduler::internal_Registration()
{
	// 'it' is fine cause looks for pairs
	for (u32 it = 0; it<Registration.size(); it++)
	{
		ItemReg&	R = Registration[it];
		if (R.OP) {
			// register
			// search for paired "unregister"
			bool bFoundAndErased = false;
			for (u32 pair = it + 1; pair<Registration.size(); pair++)
			{
				ItemReg&	R_pair = Registration[pair];
				if (!R_pair.OP && R_pair.Object == R.Object) {
					bFoundAndErased = true;
					Registration.erase(Registration.begin() + pair);
					break;
				}
			}

			// register if non-paired
			if (!bFoundAndErased) {
				internal_Register(R.Object, R.RT);
			}
		}
		else
		{
			// unregister
			internal_Unregister(R.Object, R.RT);
		}
	}
	Registration.clear();
}

void CSheduler::internal_Register(ISheduled* O, BOOL RT)
{
	VERIFY(!O->shedule.b_locked);
	if (RT)
	{
		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute = Device.dwTimeGlobal;
		TNext.dwTimeOfLastExecute = Device.dwTimeGlobal;
		TNext.Object = O;
		TNext.scheduled_name = O->shedule_Name();
		O->shedule.b_RT = TRUE;

		ItemsRT.push_back(TNext);
	}
	else {
		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute = Device.dwTimeGlobal;
		TNext.dwTimeOfLastExecute = Device.dwTimeGlobal;
		TNext.Object = O;
		TNext.scheduled_name = O->shedule_Name();
		O->shedule.b_RT = FALSE;

		// Insert into priority Queue
		Push(TNext);
	}
}

bool CSheduler::internal_Unregister(ISheduled* O, BOOL RT, bool warn_on_not_found)
{
	//the object may be already dead
	//VERIFY	(!O->shedule.b_locked)	;
	if (RT)
	{
		for (u32 i = 0; i<ItemsRT.size(); i++)
		{
			if (ItemsRT[i].Object == O) {
				ItemsRT.erase(ItemsRT.begin() + i);
				return				(true);
			}
		}
	}
	else {
		for (Item& ItemIter : Items)
		{
			if (ItemIter.Object == O) {
				ItemIter.Object = nullptr;
				return				(true);
			}
		}
	}
	if (m_current_step_obj == O)
	{
		m_current_step_obj = nullptr;
		return true;
	}

	return							(false);
}

bool CSheduler::Registered(ISheduled *object) const
{
	u32							count = 0;
	using ITEMS = xr_vector<Item>;
	using ITEMS_REG = xr_vector<ItemReg>;

	for (const Item& ItemIter : ItemsRT)
	{
		if (ItemIter.Object == object)
		{
			count = 1;
			break;
		}
	}

	for (const Item& ItemIter : Items)
	{
		if (ItemIter.Object == object)
		{
			VERIFY(!count);
			count = 1;
			break;
		}
	}

	for (const Item& ItemIter : ItemsProcessed)
	{
		if (ItemIter.Object == object)
		{
			VERIFY(!count);
			count = 1;
			break;
		}
	}

	for (const ItemReg& ItemIter : Registration)
	{
		if (ItemIter.Object == object)
		{
			if (ItemIter.OP)
			{
				VERIFY(!count);
				++count;
			}
			else
			{
				VERIFY(count == 1);
				--count;
			}
		}
	}

	if (!count && (m_current_step_obj == object))
	{
		VERIFY2(m_processing_now, "trying to unregister self unregistering object while not processing now");
		count = 1;
	}

	VERIFY(!count || (count == 1));
	return						(count == 1);
}

void	CSheduler::Register(ISheduled* A, BOOL RT)
{
	VERIFY(!Registered(A));

	ItemReg		R;
	R.OP = TRUE;
	R.RT = RT;
	R.Object = A;
	R.Object->shedule.b_RT = RT;

	Registration.push_back(R);
}

void	CSheduler::Unregister(ISheduled* A)
{
	//VERIFY(Registered(A));

	if (!Registered(A)) return;

	if (m_processing_now) {
		if (internal_Unregister(A, A->shedule.b_RT, false))
			return;
	}

	ItemReg		R;
	R.OP = FALSE;
	R.RT = A->shedule.b_RT;
	R.Object = A;

	Registration.push_back(R);
}

void CSheduler::EnsureOrder(ISheduled* Before, ISheduled* After)
{
	VERIFY(Before->shedule.b_RT && After->shedule.b_RT);

	for (u32 i = 0; i<ItemsRT.size(); i++)
	{
		if (ItemsRT[i].Object == After)
		{
			Item	A = ItemsRT[i];
			ItemsRT.erase(ItemsRT.begin() + i);
			ItemsRT.push_back(A);
			return;
		}
	}
}

void CSheduler::Push(Item& I)
{
	Items.push_back(I);
	std::push_heap(Items.begin(), Items.end());
}

void CSheduler::Pop()
{
	std::pop_heap(Items.begin(), Items.end());
	Items.pop_back();
}

void CSheduler::ProcessStep()
{
	// Normal priority
	u32	dwTime = Device.dwTimeGlobal;

	while (!Items.empty() && Top().dwTimeForExecute < dwTime)
	{
		// Update
		Item	T = Top();
		u32		Elapsed = dwTime - T.dwTimeOfLastExecute;

		// Insert into priority Queue
		Pop();

		// Erase element
		if ((!T.Object || !T.Object->shedule_Needed()))
		{
			continue;
		}

#ifdef DEBUG
		VERIFY(T.Object->dbg_startframe != Device.dwFrame);
		T.Object->dbg_startframe = Device.dwFrame;
#endif

		// Calc next update interval
		u32		dwMin = std::max(u32(30), T.Object->shedule.t_min); 
		u32		dwMax = (1000 + T.Object->shedule.t_max) / 2;
		float	scale = T.Object->shedule_Scale();
		u32		dwUpdate = dwMin + iFloor(float(dwMax - dwMin)*scale);
		clamp(dwUpdate, u32(std::max(dwMin, u32(20))), dwMax);

		m_current_step_obj = T.Object;

		u32 dt = clampr(Elapsed, u32(1), u32(std::max(u32(T.Object->shedule.t_max), u32(1000))));
		{
			CProfileSchedule scheduleProfile(T.Object->shedule_Class_Name());
			
			T.Object->shedule_Update(dt);
		}

		m_current_step_obj = nullptr;

		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute = dwTime + dwUpdate;
		TNext.dwTimeOfLastExecute = dwTime;
		TNext.Object = T.Object;
		TNext.scheduled_name = T.Object->shedule_Name();
		ItemsProcessed.push_back(TNext);
	}

	// Push "processed" back
	while (!ItemsProcessed.empty()) {
		Push(ItemsProcessed.back());
		ItemsProcessed.pop_back();
	}
}

void CSheduler::Update()
{	
	R_ASSERT(Device.Statistic);

	// Initialize
	internal_Registration();
	g_bSheduleInProgress = TRUE;

	// Realtime priority
	m_processing_now = true;
	u32	dwTime = Device.dwTimeGlobal;

	for (CSheduler::Item& item : ItemsRT)
	{
		R_ASSERT(item.Object);

#ifdef DEBUG
		VERIFY(item.Object->dbg_startframe != Device.dwFrame);
		item.Object->dbg_startframe = Device.dwFrame;
#endif

		if (!item.Object->shedule_Needed())
		{
			item.dwTimeOfLastExecute = dwTime;
			continue;
		}

		u32	Elapsed = dwTime - item.dwTimeOfLastExecute;

		item.Object->shedule_Update(Elapsed);
		item.dwTimeOfLastExecute = dwTime;
	}

	// Normal (sheduled)
	ProcessStep();
	m_processing_now = false;

	// Finalize
	g_bSheduleInProgress = FALSE;
	internal_Registration();

}
