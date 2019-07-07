#include "stdafx.h"
#include "net_client.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#include <dxerr/dxerr.h>
#pragma warning(pop)

INetQueue::INetQueue()		
{
	unused.reserve	(128);
	for (int i=0; i<16; i++)
		unused.push_back	(xr_new<NET_Packet>());
}

INetQueue::~INetQueue()
{
	xrCriticalSectionGuard guard(cs);
	u32				it;
	for				(it=0; it<unused.size(); it++)	xr_delete(unused[it]);
	for				(it=0; it<ready.size(); it++)	xr_delete(ready[it]);
}

static u32 LastTimeCreate = 0;
NET_Packet*		INetQueue::Create	()
{
	NET_Packet*	P = nullptr;
	if (unused.empty())	
	{
		ready.push_back		(xr_new<NET_Packet> ());
		P					= ready.back	();
		//---------------------------------------------
		LastTimeCreate = GetTickCount();
		//---------------------------------------------
	} else {
		ready.push_back		(unused.back());
		unused.pop_back		();
		P					= ready.back	();
	}
	//cs.Leave		();
	return	P;
}

NET_Packet*		INetQueue::Create	(const NET_Packet& _other)
{
	NET_Packet*	P			= nullptr;
	xrCriticalSectionGuard guard(cs);
	if (unused.empty())	
	{
		ready.push_back		(xr_new<NET_Packet> ());
		P					= ready.back	();
		//---------------------------------------------
		LastTimeCreate = GetTickCount();
		//---------------------------------------------
	} else {
		ready.push_back		(unused.back());
		unused.pop_back		();
		P					= ready.back	();
	}	
    std::memcpy(P,&_other,sizeof(NET_Packet));
	return			P;
}

NET_Packet*		INetQueue::Retreive	()
{
	NET_Packet*	P			= nullptr;
	if (!ready.empty())		P = ready.front();
	//---------------------------------------------	
	else
	{
		u32 tmp_time = GetTickCount()-60000;
		size_t size = unused.size();
		if ((LastTimeCreate < tmp_time) &&  (size > 32))
		{
			xr_delete(unused.back());
			unused.pop_back();
		}		
	}
	//---------------------------------------------	
	return	P;
}
void			INetQueue::Release	()
{
	VERIFY			(!ready.empty());
	//---------------------------------------------
	u32 tmp_time = GetTickCount()-60000;
	size_t size = unused.size();
	ready.front()->B.count = 0;
	if ((LastTimeCreate < tmp_time) &&  (size > 32))
	{
		xr_delete(ready.front());
	}
	else
		unused.push_back(ready.front());
	//---------------------------------------------	
	ready.pop_front	();
}

//

IPureClient::IPureClient(CTimer* timer)
{
	device_timer			= timer;
}

IPureClient::~IPureClient()
{
}

void IPureClient::Disconnect() {}

void IPureClient::OnMessage(void* data, u32 size)
{
	// One of the messages - decompress it
	
	NET_Packet* P = net_Queue.Create();

	P->set( data, size );	
	P->timeReceive	= timeServer_Async();

	u16			m_type;
	P->r_begin	(m_type);
	
}
