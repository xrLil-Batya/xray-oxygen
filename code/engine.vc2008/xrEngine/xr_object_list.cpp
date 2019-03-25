#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "xrSheduler.h"
#include "xr_object_list.h"
#include "std_classes.h"

#include "xr_object.h"
#include "../xrCore/net_utils.h"
#include "../FrayBuildConfig.hpp"
#include "CustomHUD.h"
#include <ppl.h>

class fClassEQ {
	CLASS_ID cls;
public:
	fClassEQ(CLASS_ID C) : cls(C) {};
	IC bool operator() (CObject* Obj) { return cls==Obj->CLS_ID; }
};
#ifdef	DEBUG
	BOOL debug_destroy = TRUE;
#endif

CObjectList::CObjectList	( ) :
	m_owner_thread_id		(GetCurrentThreadId())
{
    std::memset(map_NETID,0,0xffff*sizeof(CObject*));
}

CObjectList::~CObjectList	( )
{
	R_ASSERT				( objects_active.empty()	);
	R_ASSERT				( objects_sleeping.empty()	);

    //#HOTFIX
    if (!destroy_queue.empty())
    {
        //Update(false);
        Msg("! Memory leaked!!!!, objects not destroyed on exit! Number of objects: %zu", destroy_queue.size());
        destroy_queue.clear();
    }

	//R_ASSERT				( destroy_queue.empty()		);
}

CObject*	CObjectList::FindObjectByName	( shared_str name )
{
	for (auto & I : objects_active)
		if (I->cName().equal(name))	return I;
	for (auto & I : objects_sleeping)
		if (I->cName().equal(name))	return I;
	return	nullptr;
}
CObject*	CObjectList::FindObjectByName	( LPCSTR name )
{
	return	FindObjectByName				(shared_str(name));
}

CObject*	CObjectList::FindObjectByCLS_ID	( CLASS_ID cls )
{
	{
		Objects::iterator Obj	= std::find_if(objects_active.begin(),objects_active.end(),fClassEQ(cls));
		if (Obj!=objects_active.end())	return *Obj;
	}
	{
		Objects::iterator Obj	= std::find_if(objects_sleeping.begin(),objects_sleeping.end(),fClassEQ(cls));
		if (Obj!=objects_sleeping.end())	return *Obj;
	}

	return	nullptr;
}


void	CObjectList::o_remove		( Objects&	v,  CObject* Obj)
{
//.	if(O->ID()==1026)
//.	{
//.		Log("ahtung");
//.	}
	Objects::iterator _i	= std::find(v.begin(),v.end(),Obj);
	VERIFY					(_i!=v.end());
	v.erase					(_i);
//.	Msg("---o_remove[%s][%d]", Obj->cName().c_str(), Obj->ID() );
}

void	CObjectList::o_activate		( CObject*		Obj		)
{
	VERIFY						(Obj && Obj->processing_enabled());
	o_remove					(objects_sleeping,Obj);
	objects_active.push_back	(Obj);
	Obj->MakeMeCrow				();
}
void	CObjectList::o_sleep		( CObject*		Obj		)
{
	VERIFY	(Obj && !Obj->processing_enabled());
	o_remove					(objects_active,Obj);
	objects_sleeping.push_back  (Obj);
	Obj->MakeMeCrow				();
}

void	CObjectList::SingleUpdate	(CObject* Obj)
{
	if (Device.dwFrame == Obj->dwFrame_UpdateCL || !Obj->processing_enabled())
		return;

	if (Obj->H_Parent())
		SingleUpdate			(Obj->H_Parent());

	Device.Statistic->UpdateClient_updated	++;
	Obj->dwFrame_UpdateCL			= Device.dwFrame;

	Obj->UpdateCL					();

	VERIFY3						(Obj->dbg_update_cl == Device.dwFrame, "Broken sequence of calls to 'UpdateCL'",*Obj->cName());

	// Push to destroy-queue if it isn't here already
	if (Obj->H_Parent() && (Obj->H_Parent()->getDestroy() || Obj->H_Root()->getDestroy()))	
		Msg	("! ERROR: incorrect destroy sequence for object[%d:%s], section[%s], parent[%d:%s]",Obj->ID(),*Obj->cName(),*Obj->cNameSect(),Obj->H_Parent()->ID(),*Obj->H_Parent()->cName());

}

void CObjectList::clear_crow_vec(Objects& obj)
{
	for (auto & _it : obj)
		_it->IAmNotACrowAnyMore();
	
	obj.clear();
}

void CObjectList::Update		(bool bForce)
{
	if ( !Device.Paused() || bForce )
	{
		// Clients
		if (Device.fTimeDelta>EPS_S || bForce)			
		{
			// Select Crow-Mode
			Device.Statistic->UpdateClient_updated	= 0;

			Objects& crows				= m_crows[0];

			{
				Objects& crows1			= m_crows[1];
				crows.insert			(crows.end(), crows1.begin(), crows1.end());
				crows1.clear	();
			}

#ifdef DEBUG
			std::sort					(crows.begin(), crows.end());
			VERIFY						(
				std::unique(
					crows.begin(),
					crows.end()
				) == crows.end()
			);
#endif // ifdef DEBUG

			Device.Statistic->UpdateClient_crows	= u32(crows.size());
			Objects* workload			= nullptr;
			if (!psDeviceFlags.test(rsDisableObjectsAsCrows))	
				workload				= &crows;
			else {
				workload				= &objects_active;
				clear_crow_vec			(crows);
			}

			size_t const objects_count	= workload->size();
			CObject** objects			= new CObject*[objects_count];
			std::copy					( workload->begin(), workload->end(), objects );

			crows.clear		();

			CObject** b					= objects;
			CObject** e					= objects + objects_count;
			for (CObject** i = b; i != e; ++i) {
				(*i)->IAmNotACrowAnyMore();
				(*i)->dwFrame_AsCrow	= u32(-1);
			}

			for (CObject** i = b; i != e; ++i)
				SingleUpdate			(*i);

			xr_delete(objects);
		}
	}

	// Destroy
	if (!destroy_queue.empty()) 
	{
		// Info
		for (auto oit : objects_active)
			for (size_t it = destroy_queue.size(); it > 0; it--)
            {	
				oit->net_Relcase(destroy_queue[it - 1]);
			}

		for (auto oit : objects_sleeping)
			for (size_t it = destroy_queue.size(); it > 0; it--)	
				oit->net_Relcase(destroy_queue[it - 1]);

		for (size_t it = destroy_queue.size(); it > 0; it--)
			Sound->object_relcase	(destroy_queue[it - 1]);
		
		for(auto It : m_relcase_callbacks)
		{
			//VERIFY (*It.m_ID==(It-m_relcase_callbacks.begin()));
			for (auto dIt: destroy_queue)
			{
				It.m_Callback(dIt);
				g_hud->net_Relcase(dIt);
			}
		}

		// Destroy
		for (size_t it = destroy_queue.size(); it > 0; it--)
		{
			CObject*		Obj	= destroy_queue[it - 1];
			Obj->net_Destroy	( );
			Destroy			(Obj);
		}
		destroy_queue.clear	();
	}
}

void CObjectList::net_Register		(CObject* Obj)
{
	R_ASSERT		(Obj);
	R_ASSERT		(Obj->ID() < 0xffff);

	map_NETID[Obj->ID()] = Obj;
}

void CObjectList::net_Unregister	(CObject* Obj)
{
	if (Obj->ID() < 0xffff)				//demo_spectator can have 0xffff
		map_NETID[Obj->ID()] = nullptr;
}

int	g_Dump_Export_Obj = 0;

u32	CObjectList::net_Export			(NET_Packet* _Packet,	u32 start, u32 max_object_size	)
{
	if (g_Dump_Export_Obj) Msg("---- net_export --- ");

	NET_Packet& Packet	= *_Packet;
	for (; start<objects_active.size() + objects_sleeping.size(); start++)			{
		CObject* P = (start<objects_active.size()) ? objects_active[start] : objects_sleeping[start-objects_active.size()];
		if (P->net_Relevant() && !P->getDestroy())	{			
			Packet.w_u16			(u16(P->ID())	);
			u32 position;
			Packet.w_chunk_open8	(position);
			P->net_Export			(Packet);

			if (g_Dump_Export_Obj)
			{
				u32 size				= u32		(Packet.w_tell()-position)-sizeof(u8);
				Msg("* %s : %d", *(P->cNameSect()), size);
			}
			Packet.w_chunk_close8	(position);
			
			if (max_object_size >= (NET_PacketSizeLimit - Packet.w_tell()))
				break;
		}
	}
	if (g_Dump_Export_Obj) Msg("------------------- ");
	return	start+1;
}

void CObjectList::Load		()
{
	R_ASSERT				(objects_active.empty() && destroy_queue.empty() && objects_sleeping.empty());
}

void CObjectList::Unload	( )
{
	if (!objects_sleeping.empty() || !objects_active.empty())
		Msg			("! objects-leaked: %d",objects_sleeping.size() + objects_active.size());

	// Destroy objects
	while (!objects_sleeping.empty())
	{
		CObject*	Obj	= objects_sleeping.back	();
		Msg				("! [%x] s[%4d]-[%s]-[%s]", Obj, Obj->ID(), *Obj->cNameSect(), *Obj->cName());
		Obj->setDestroy	( true );
		
#ifdef DEBUG
		if( debug_destroy )
			Msg				("Destroying object [%d][%s]",Obj->ID(),*Obj->cName());
#endif
		Obj->net_Destroy	(   );
		Destroy			( Obj );
	}
	while (!objects_active.empty())
	{
		CObject*	Obj	= objects_active.back	();
		Msg				("! [%x] a[%4d]-[%s]-[%s]", Obj, Obj->ID(), *Obj->cNameSect(), *Obj->cName());
		Obj->setDestroy	( true );

#ifdef DEBUG
		if( debug_destroy )
			Msg				("Destroying object [%d][%s]",Obj->ID(),*Obj->cName());
#endif
		Obj->net_Destroy	(   );
		Destroy			( Obj );
	}
}

CObject*	CObjectList::Create				( LPCSTR	name	)
{
	CObject*	O				= g_pGamePersistent->ObjectPool.create(name);
//	Msg("CObjectList::Create [%x]%s", Obj, name);
	objects_sleeping.push_back	(O);
	return						O;
}

void		CObjectList::Destroy			( CObject*	Obj		)
{
	if (nullptr==Obj)								return;
	net_Unregister							(Obj);

	if (!Device.Paused())
	{
		if (!m_crows[1].empty())
		{
			Objects::const_iterator i = m_crows[1].begin();
			Objects::const_iterator	const e = m_crows[1].end();
#ifdef DEBUG
			u32 iter = 0;
			for (CObject* pCrow : m_crows[1])
			{
				Msg("m_crows[1] object: %d %s", iter, pCrow->cName().c_str());
				iter++;
			}
#endif
			m_crows[1].clear();
		}
	}
	else {
		Objects& crows				= m_crows[1];
		Objects::iterator const i	= std::find(crows.begin(),crows.end(),Obj);
		if	( i != crows.end() ) {
			crows.erase				( i);
			VERIFY					( std::find(crows.begin(), crows.end(),Obj) == crows.end() );
		}
	}

	Objects& crows				= m_crows[0];
	Objects::iterator _i0		= std::find(crows.begin(),crows.end(),Obj);
	if	(_i0!=crows.end()) {
		crows.erase				(_i0);
		VERIFY					( std::find(crows.begin(), crows.end(),Obj) == crows.end() );
	}

	// active/inactive
	Objects::iterator _i		= std::find(objects_active.begin(),objects_active.end(),Obj);
	if	(_i!=objects_active.end()) {
		objects_active.erase				(_i);
		VERIFY								( std::find(objects_active.begin(),objects_active.end(),Obj) == objects_active.end() );
		VERIFY								(
			std::find(
				objects_sleeping.begin(),
				objects_sleeping.end(),
				Obj
			) == objects_sleeping.end()
		);
	}
	else {
		Objects::iterator _ii	= std::find(objects_sleeping.begin(),objects_sleeping.end(),Obj);
		if	(_ii!=objects_sleeping.end()) {
			objects_sleeping.erase			(_ii);
			VERIFY							( std::find(objects_sleeping.begin(),objects_sleeping.end(),Obj) == objects_sleeping.end() );
		}
		else
			FATAL							("! Unregistered object being destroyed");
	}

	g_pGamePersistent->ObjectPool.destroy	(Obj);
}

void CObjectList::relcase_register		(RELCASE_CALLBACK cb, int *ID)
{
#ifdef DEBUG
	RELCASE_CALLBACK_VEC::iterator It = std::find(	m_relcase_callbacks.begin(),
													m_relcase_callbacks.end(),
													cb);
	VERIFY(It==m_relcase_callbacks.end());
#endif
	*ID								= (int)m_relcase_callbacks.size();
	m_relcase_callbacks.emplace_back	(ID,cb);
}

void CObjectList::relcase_unregister	(int* ID)
{
	VERIFY							(m_relcase_callbacks[*ID].m_ID==ID);
	m_relcase_callbacks[*ID]		= m_relcase_callbacks.back();
	*m_relcase_callbacks.back().m_ID= *ID;
	m_relcase_callbacks.pop_back	();
}

void CObjectList::dump_list(Objects& v, LPCSTR reason)
{
#ifdef DEBUG
	Objects::iterator it = v.begin();
	Objects::iterator it_e = v.end();
	Msg("----------------dump_list [%s]",reason);
	for(;it!=it_e;++it)
		Msg("%x - name [%s] ID[%d] parent[%s] getDestroy()=[%s]", 
			(*it),
			(*it)->cName().c_str(), 
			(*it)->ID(), 
			((*it)->H_Parent())?(*it)->H_Parent()->cName().c_str():"", 
			((*it)->getDestroy())?"yes":"no" );
#endif // #ifdef DEBUG
}

bool CObjectList::dump_all_objects()
{ 
#ifdef DEBUG
    if (strstr(Core.Params, "-dump_list"))
    {
	    dump_list(destroy_queue,"destroy_queue");
	    dump_list(objects_active,"objects_active");
	    dump_list(objects_sleeping,"objects_sleeping");
	    dump_list(m_crows[0],"m_crows[0]");
	    dump_list(m_crows[1],"m_crows[1]");
    }
#endif
	return false;
}

void CObjectList::register_object_to_destroy(CObject *object_to_destroy)
{
	VERIFY					(!registered_object_to_destroy(object_to_destroy));
//	Msg("CObjectList::register_object_to_destroy [%x]", object_to_destroy);
	destroy_queue.push_back	(object_to_destroy);

	Objects::iterator it	= objects_active.begin();
	Objects::iterator it_e	= objects_active.end();
	for(;it!=it_e;++it)
	{
		CObject* Obj = *it;
		if(!Obj->getDestroy() && Obj->H_Parent()==object_to_destroy)
		{
			Msg("setDestroy called, but not-destroyed child found parent[%d] child[%d]",object_to_destroy->ID(), Obj->ID(), Device.dwFrame);
			Obj->setDestroy(TRUE);
		}
	}

	it		= objects_sleeping.begin();
	it_e	= objects_sleeping.end();
	for(;it!=it_e;++it)
	{
		CObject* Obj = *it;
		if(!Obj->getDestroy() && Obj->H_Parent()==object_to_destroy)
		{
			Msg("setDestroy called, but not-destroyed child found parent[%d] child[%d]",object_to_destroy->ID(), Obj->ID(), Device.dwFrame);
			Obj->setDestroy(TRUE);
		}
	}
}

#ifdef DEBUG
bool CObjectList::registered_object_to_destroy	(const CObject *object_to_destroy) const
{
	return					(
		std::find(
			destroy_queue.begin(),
			destroy_queue.end(),
			object_to_destroy
		) != 
		destroy_queue.end()
	);
}
#endif // DEBUG
