#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"

//#define DEBUG_SCHEDULER

float			psShedulerCurrent		= 10.f	;
float			psShedulerTarget		= 10.f	;
const	float	psShedulerReaction		= 0.1f	;
BOOL			g_bSheduleInProgress	= FALSE	;

//-------------------------------------------------------------------------------------
void CSheduler::Initialize		()
{
	m_current_step_obj	= nullptr;
	m_processing_now	= false;
}

void CSheduler::Destroy			()
{
	internal_Registration		();

	for (u32 it=0; it<Items.size(); it++)
	{
		if (nullptr==Items[it].Object)	
		{
			Items.erase(Items.begin()+it);
			it	--;
		}
	}
#ifdef DEBUG	
	if (!Items.empty())
	{
		string1024		_objects; _objects[0]=0;

		Msg				("! Sheduler work-list is not empty");
		for (u32 it=0; it<Items.size(); it++)
			Msg("%s", Items[it].Object->shedule_Name().c_str());
	}
#endif // DEBUG
	ItemsRT.clear		();
	Items.clear			();
	ItemsProcessed.clear();
	Registration.clear	();
}

void	CSheduler::internal_Registration()
{
	for (u32 it=0; it<Registration.size(); it++)
	{
		ItemReg&	R	= Registration	[it];
		if (R.OP)	{
			// register
			// search for paired "unregister"
			BOOL	bFoundAndErased		= FALSE;
			for (u32 pair=it+1; pair<Registration.size(); pair++)
			{
				ItemReg&	R_pair	= Registration	[pair];
				if	((!R_pair.OP)&&(R_pair.Object == R.Object))	{
					bFoundAndErased		= TRUE;
					Registration.erase	(Registration.begin()+pair	);
					break				;
				}
			}

			// register if non-paired
			if (!bFoundAndErased)		{
#ifdef DEBUG_SCHEDULER
				Msg						("SCHEDULER: internal register [%s][%x][%s]",*R.Object->shedule_Name(),R.Object,R.RT ? "true" : "false");
#endif // DEBUG_SCHEDULER
				internal_Register		(R.Object,R.RT);
			}
#ifdef DEBUG_SCHEDULER
			else 
				Msg						("SCHEDULER: internal register skipped, because unregister found [%s][%x][%s]","unknown",R.Object,R.RT ? "true" : "false");
#endif // DEBUG_SCHEDULER
		}
		else		{
			// unregister
			internal_Unregister			(R.Object,R.RT);
		}
	}
	Registration.clear	();
}

void CSheduler::internal_Register	(ISheduled* Ish, BOOL RT)
{
	VERIFY	(!Ish->shedule.b_locked)	;
	if (RT)
	{
		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute		= Device.dwTimeGlobal;
		TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
		TNext.Object				= Ish;
		TNext.scheduled_name		= Ish->shedule_Name();
		Ish->shedule.b_RT				= TRUE;

		ItemsRT.push_back			(TNext);
	} else {
		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute		= Device.dwTimeGlobal;
		TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
		TNext.Object				= Ish;
		TNext.scheduled_name		= Ish->shedule_Name();
		Ish->shedule.b_RT				= FALSE;

		// Insert into priority Queue
		Push						(TNext);
	}
}

bool CSheduler::internal_Unregister	(ISheduled* Ish, BOOL RT, bool warn_on_not_found)
{
	//the object may be already dead
	//VERIFY	(!Ish->shedule.b_locked)	;
	if (RT)
	{
		for (u32 i=0; i<ItemsRT.size(); i++)
		{
			if (ItemsRT[i].Object == Ish) {
#ifdef DEBUG_SCHEDULER
				Msg					("SCHEDULER: internal unregister [%s][%x][%s]","unknown",Ish,"true");
#endif // DEBUG_SCHEDULER
				ItemsRT.erase(ItemsRT.begin()+i);
				return				(true);
			}
		}
	} else {
		for (auto & Item : Items)
		{
			if (Item.Object == Ish) {
#ifdef DEBUG_SCHEDULER
				Msg					("SCHEDULER: internal unregister [%s][%x][%s]",*Items[i].scheduled_name,Ish,"false");
#endif // DEBUG_SCHEDULER
				Item.Object	= nullptr;
				return				(true);
			}
		}
	}
	if (m_current_step_obj == Ish)
	{
#ifdef DEBUG_SCHEDULER
		Msg					("SCHEDULER: internal unregister (self unregistering) [%x][%s]",Ish,"false");
#endif // DEBUG_SCHEDULER

		m_current_step_obj = nullptr;
		return true;
	}

#ifdef DEBUG
	if (warn_on_not_found)
		Msg							("! scheduled object %s tries to unregister but is not registered",*Ish->shedule_Name());
#endif // DEBUG

	return							(false);
}

bool CSheduler::Registered		(ISheduled *object) const
{
	u32							count = 0;
	using ITEMS =  xr_vector<Item>;

	{
		ITEMS::const_iterator	I = ItemsRT.begin();
		ITEMS::const_iterator	E = ItemsRT.end();
		for ( ; I != E; ++I)
			if ((*I).Object == object) {
//				Msg				("0x%8x found in RT",object);
				count			= 1;
				break;
			}
	}
	{
		ITEMS::const_iterator	I = Items.begin();
		ITEMS::const_iterator	E = Items.end();
		for ( ; I != E; ++I)
			if ((*I).Object == object) {
//				Msg				("0x%8x found in non-RT",object);
				VERIFY			(!count);
				count			= 1;
				break;
			}
	}

	{
		ITEMS::const_iterator	I = ItemsProcessed.begin();
		ITEMS::const_iterator	E = ItemsProcessed.end();
		for ( ; I != E; ++I)
			if ((*I).Object == object) {
//				Msg				("0x%8x found in process items",object);
				VERIFY			(!count);
				count			= 1;
				break;
			}
	}

	using ITEMS_REG = xr_vector<ItemReg>;
	ITEMS_REG::const_iterator	I = Registration.begin();
	ITEMS_REG::const_iterator	E = Registration.end();
	for ( ; I != E; ++I) {
		if ((*I).Object == object) {
			if ((*I).OP) {
//				Msg				("0x%8x found in registration on register",object);
				VERIFY			(!count);
				++count;
			}
			else {
//				Msg				("0x%8x found in registration on UNregister",object);
				VERIFY			(count == 1);
				--count;
			}
		}
	}

	if (!count && (m_current_step_obj == object))
	{
		VERIFY2(m_processing_now, "trying to unregister self unregistering object while not processing now");
		count = 1;
	}
	VERIFY						(!count || (count == 1));
	return						(count == 1);
}

void	CSheduler::Register		(ISheduled* A, BOOL RT				)
{
	VERIFY		(!Registered(A));

	ItemReg		R;
	R.OP		= TRUE				;
	R.RT		= RT				;
	R.Object	= A					;
	R.Object->shedule.b_RT	= RT	;

#ifdef DEBUG_SCHEDULER
	Msg			("SCHEDULER: register [%s][%x]",*A->shedule_Name(),A);
#endif // DEBUG_SCHEDULER

	Registration.push_back	(R);
}

void	CSheduler::Unregister	(ISheduled* A						)
{
	VERIFY		(Registered(A));

#ifdef DEBUG_SCHEDULER
	Msg			("SCHEDULER: unregister [%s][%x]",*A->shedule_Name(),A);
#endif // DEBUG_SCHEDULER

	if (m_processing_now) {
		if (internal_Unregister(A,A->shedule.b_RT,false))
			return;
	}

	ItemReg		R;
	R.OP		= FALSE				;
	R.RT		= A->shedule.b_RT	;
	R.Object	= A					;

	Registration.push_back			(R);
}

void CSheduler::EnsureOrder		(ISheduled* Before, ISheduled* After)
{
	VERIFY(Before->shedule.b_RT && After->shedule.b_RT);

	for (u32 i=0; i<ItemsRT.size(); i++)
	{
		if (ItemsRT[i].Object==After) 
		{
			Item	A			= ItemsRT[i];
			ItemsRT.erase		(ItemsRT.begin()+i);
			ItemsRT.push_back	(A);
			return;
		}
	}
}

void CSheduler::Push				(Item& I)
{
	Items.push_back	(I);
	std::push_heap	(Items.begin(), Items.end());
}

void CSheduler::Pop					()
{
	std::pop_heap	(Items.begin(), Items.end());
	Items.pop_back	();
}

void CSheduler::ProcessStep			()
{
	// Normal priority
	u32		dwTime					= Device.dwTimeGlobal;
	CTimer							eTimer;
	for (int i=0;!Items.empty() && Top().dwTimeForExecute < dwTime; ++i) {
		u32		delta_ms			= dwTime - Top().dwTimeForExecute;

		// Update
		Item	T					= Top	();
#ifdef DEBUG_SCHEDULER
		Msg		("SCHEDULER: process step [%s][%x][false]",*T.scheduled_name,T.Object);
#endif // DEBUG_SCHEDULER
		u32		Elapsed				= dwTime-T.dwTimeOfLastExecute;
		bool	condition;
		
		condition					= (nullptr == T.Object || !T.Object->shedule_Needed());
		if (condition) {
			// Erase element
#ifdef DEBUG_SCHEDULER
			Msg						("SCHEDULER: process unregister [%s][%x][%s]",*T.scheduled_name,T.Object,"false");
#endif // DEBUG_SCHEDULER
//			if (T.Object)
//				Msg					("0x%08x UNREGISTERS because shedule_Needed() returned false",T.Object);
//			else
//				Msg					("UNREGISTERS unknown object");
			Pop						();
			continue;
		}

		// Insert into priority Queue
		Pop							();

		// Real update call
		// Msg						("------- %d:",Device.dwFrame);
#ifdef DEBUG
		T.Object->dbg_startframe	= Device.dwFrame;
		eTimer.Start				();
//		LPCSTR		_obj_name		= T.Object->shedule_Name().c_str();
#endif // DEBUG

		// Calc next update interval
		u32		dwMin				= std::max(u32(30),T.Object->shedule.t_min);
		u32		dwMax				= (1000+T.Object->shedule.t_max)/2;
		float	scale				= T.Object->shedule_Scale	(); 
		u32		dwUpdate			= dwMin+iFloor(float(dwMax-dwMin)*scale);
		clamp	(dwUpdate,u32(std::max(dwMin,u32(20))),dwMax);

		

		m_current_step_obj = T.Object;
//			try {
			T.Object->shedule_Update	(clampr(Elapsed,u32(1),u32(std::max(u32(T.Object->shedule.t_max),u32(1000)))) );
			if (!m_current_step_obj)
			{
#ifdef DEBUG_SCHEDULER
				Msg						("SCHEDULER: process unregister (self unregistering) [%s][%x][%s]",*T.scheduled_name,T.Object,"false");
#endif // DEBUG_SCHEDULER
				continue;
			}
//			} catch (...) {
#ifdef DEBUG
//				Msg		("! xrSheduler: object '%s' raised an exception", _obj_name);
//				throw	;
#endif // DEBUG
//			}
		m_current_step_obj = nullptr;

#ifdef DEBUG
//		u32	execTime				= eTimer.GetElapsed_ms		();
#endif // DEBUG

		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute		= dwTime+dwUpdate;
		TNext.dwTimeOfLastExecute	= dwTime;
		TNext.Object				= T.Object;
		TNext.scheduled_name		= T.Object->shedule_Name();
		ItemsProcessed.push_back	(TNext);


#ifdef DEBUG
//		u32	execTime				= eTimer.GetElapsed_ms		();
		// VERIFY3					(T.Object->dbg_update_shedule == T.Object->dbg_startframe, "Broken sequence of calls to 'shedule_Update'", _obj_name );
		if (delta_ms> 3*dwUpdate)	{
			//Msg	("! xrSheduler: failed to shedule object [%s] (%dms)",	_obj_name, delta_ms	);
		}
//		if (execTime> 15)			{
//			Msg	("* xrSheduler: too much time consumed by object [%s] (%dms)",	_obj_name, execTime	);
//		}
#endif // DEBUG

		// 
		if ((i % 3) != (3 - 1))
			continue;

		if (Device.dwPrecacheFrame==0 && CPU::QPC() > cycles_limit)		
		{
			// we have maxed out the load - increase heap
			psShedulerTarget		+= (psShedulerReaction * 3);
			break;
		}
	}

	// Push "processed" back
	while (!ItemsProcessed.empty())	{
		Push	(ItemsProcessed.back())	;
		ItemsProcessed.pop_back		()	;
	}

	// always try to decrease target
	psShedulerTarget	-= psShedulerReaction;
}
/*
void CSheduler::Switch				()
{
	if (fibered)	
	{
		fibered						= FALSE;
		SwitchToFiber				(fiber_main);
	}
}
*/
void CSheduler::Update				()
{
	R_ASSERT						(Device.Statistic);
	// Initialize
	Device.Statistic->Sheduler.Begin();
	cycles_start					= CPU::QPC			();
	cycles_limit					= CPU::qpc_freq * u64 (iCeil(psShedulerCurrent)) / 1000i64 + cycles_start;
	internal_Registration			();
	g_bSheduleInProgress			= TRUE;

#ifdef DEBUG_SCHEDULER
	Msg								("SCHEDULER: PROCESS STEP %d",Device.dwFrame);
#endif // DEBUG_SCHEDULER
	// Realtime priority
	m_processing_now				= true;
	u32	dwTime						= Device.dwTimeGlobal;
	for (u32 it=0; it<ItemsRT.size(); it++)
	{
		Item&	T					= ItemsRT[it];
		R_ASSERT					(T.Object);
#ifdef DEBUG_SCHEDULER
		Msg							("SCHEDULER: process step [%s][%x][true]",*T.Object->shedule_Name(),T.Object);
#endif // DEBUG_SCHEDULER
		if(!T.Object->shedule_Needed()){
#ifdef DEBUG_SCHEDULER
			Msg						("SCHEDULER: process unregister [%s][%x][%s]",*T.Object->shedule_Name(),T.Object,"false");
#endif // DEBUG_SCHEDULER
			T.dwTimeOfLastExecute	= dwTime;
			continue;
		}

		u32	Elapsed					= dwTime-T.dwTimeOfLastExecute;
#ifdef DEBUG
		VERIFY						(T.Object->dbg_startframe != Device.dwFrame);
		T.Object->dbg_startframe	= Device.dwFrame;
#endif
		T.Object->shedule_Update	(Elapsed);
		T.dwTimeOfLastExecute		= dwTime;
	}

	// Normal (sheduled)
	ProcessStep						();
	m_processing_now				= false;
#ifdef DEBUG_SCHEDULER
	Msg								("SCHEDULER: PROCESS STEP FINISHED %d",Device.dwFrame);
#endif // DEBUG_SCHEDULER
	clamp							(psShedulerTarget,3.f,66.f);
	psShedulerCurrent				= 0.9f*psShedulerCurrent + 0.1f*psShedulerTarget;
	Device.Statistic->fShedulerLoad	= psShedulerCurrent;

	// Finalize
	g_bSheduleInProgress			= FALSE;
	internal_Registration			();
	Device.Statistic->Sheduler.End	();
}
