#include "stdafx.h"
#include "NET_Common.h"
#include "net_client.h"
#include "net_server.h"
#include "net_messages.h"

//#include "../xrGameSpy/xrGameSpy_MainDefs.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#include <dxerr/dxerr.h>
//#pragma warning(pop)

// {0218FA8B-515B-4bf2-9A5F-2F079D1759F3}
static const GUID NET_GUID = 
{ 0x218fa8b,  0x515b, 0x4bf2, { 0x9a, 0x5f, 0x2f, 0x7, 0x9d, 0x17, 0x59, 0xf3 } };

// {8D3F9E5E-A3BD-475b-9E49-B0E77139143C}
static const GUID CLSID_NETWORKSIMULATOR_DP8SP_TCPIP =
{ 0x8d3f9e5e, 0xa3bd, 0x475b, { 0x9e, 0x49, 0xb0, 0xe7, 0x71, 0x39, 0x14, 0x3c } };

// CLSIDs added for DirectX 9
/****************************************************************************
 *
 * DirectPlay8 Interface IIDs
 *
 ****************************************************************************/

typedef REFIID	DP8REFIID;
void dump_URL(LPCSTR p, IDirectPlay8Address* A)
{
	string256	aaaa;
	DWORD		aaaa_s			= sizeof(aaaa);
	R_CHK		(A->GetURLA(aaaa,&aaaa_s));
	Log			(p,aaaa);
}

// 
INetQueue::INetQueue()		
{
	unused.reserve	(128);
	for (int i=0; i<16; i++)
		unused.push_back	(xr_new<NET_Packet>());
}

INetQueue::~INetQueue()
{
    std::lock_guard<decltype(cs)> lock(cs);
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
	NET_Packet*	P			= 0;
    std::lock_guard<decltype(cs)> lock(cs);
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
	NET_Packet*	P			= 0;
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
const u32 syncQueueSize		= 512;
const int syncSamples		= 256;
class ENGINE_API syncQueue
{
	u32				table	[syncQueueSize];
	u32				write;
	u32				count;
public:
	syncQueue()			{ clear(); }

	IC void			push	(u32 value)
	{
		table[write++]	= value;
		if (write == syncQueueSize)	write = 0;

		if (count <= syncQueueSize)	count++;
	}
	IC u32*		begin	()	{ return table;			}
	IC u32*		end		()	{ return table+count;	}
	IC u32		size	()	{ return count;			}
	IC void     clear	()	{ write=0; count=0;		}
} net_DeltaArray;

//-------
ENGINE_API Flags32	psNET_Flags			= {0};
ENGINE_API int		psNET_ClientUpdate	= 30;		// FPS
ENGINE_API int		psNET_ClientPending	= 2;
ENGINE_API char	psNET_Name[32]		= "Player";
ENGINE_API BOOL	psNET_direct_connect = FALSE;

/****************************************************************************
 * DirectPlay8 Service Provider GUIDs
 ****************************************************************************/
//------------------------------------------------------------------------------
void IPureClient::_SendTo_LL( const void* data, u32 size, u32 flags, u32 timeout )
{
    IPureClient::SendTo_LL( const_cast<void*>(data), size, flags, timeout );
}
//==============================================================================

IPureClient::IPureClient(CTimer* timer): net_Statistic(timer)
{
	NET						= NULL;
	net_Address_server		= NULL;
	net_Address_device		= NULL;
	device_timer			= timer;
	net_TimeDelta_User		= 0;
	net_Time_LastUpdate		= 0;
	net_TimeDelta			= 0;
	net_TimeDelta_Calculated = 0;
}

IPureClient::~IPureClient()
{
}

bool IPureClient::Connect(LPCSTR)
{
	net_Disconnected = false;
	net_TimeDelta = 0;
	return true;
}

void IPureClient::Disconnect()
{
	if( NET )	NET->Close(0);

    // Clean up Host _list_
	net_csEnumeration.lock			();
	for (u32 i=0; i<net_Hosts.size(); i++) {
		HOST_NODE&	N = net_Hosts[i];
		_RELEASE	(N.pHostAddress);
	}
	net_Hosts.clear					();
	net_csEnumeration.unlock			();

	// Release interfaces
	_SHOW_REF	("cl_netADR_Server",net_Address_server);
	_RELEASE	(net_Address_server);
	_SHOW_REF	("cl_netADR_Device",net_Address_device);
	_RELEASE	(net_Address_device);
	_SHOW_REF	("cl_netCORE",NET);
	_RELEASE	(NET);

	net_Connected = EnmConnectionWait;
	net_Syncronised = FALSE;
}

void IPureClient::OnMessage(void* data, u32 size)
{
	// One of the messages - decompress it
	net_Queue.Lock();
	NET_Packet* P = net_Queue.Create();

	P->set( data, size );	
	P->timeReceive	= timeServer_Async();//TimerAsync				(device_timer);	

	u16			m_type;
	P->r_begin	(m_type);
	net_Queue.Unlock();
}

void	IPureClient::timeServer_Correct(u32 sv_time, u32 cl_time)
{
	u32		ping	= net_Statistic.getPing();
	u32		delta	= sv_time + ping/2 - cl_time;
	net_DeltaArray.push	(delta);
	Sync_Average		();
}

void	IPureClient::SendTo_LL(void* data, u32 size, u32 dwFlags, u32 dwTimeout)
{
	if(net_Disconnected) return;

	DPN_BUFFER_DESC				desc;

	desc.dwBufferSize   = size;
	desc.pBufferData    = (BYTE*)data;

    net_Statistic.dwBytesSended	+= size;
	

	// verify
	VERIFY(desc.dwBufferSize);
	VERIFY(desc.pBufferData);
	VERIFY(NET);

    DPNHANDLE	hAsync  = 0;
	HRESULT		hr      = NET->Send( &desc, 1, dwTimeout, 0, &hAsync, dwFlags | DPNSEND_COALESCE );
	
	if(FAILED(hr))	
	{
		Msg	("! ERROR: Failed to send net-packet, reason: %s",::Debug.error2string(hr));
		DXTRACE_ERR(L"", hr);
	}
}

void IPureClient::Send( NET_Packet& packet, u32 dwFlags, u32 dwTimeout )
{
    MultipacketSender::SendPacket( packet.B.data, (u32)packet.B.count, dwFlags, dwTimeout );
}

void IPureClient::Flush_Send_Buffer		()
{
    MultipacketSender::FlushSendBuffer( 0 );
}

BOOL IPureClient::net_HasBandwidth()
{
	u32 dwTime = TimeGlobal(device_timer);
	u32 dwInterval = 0;
	if (net_Disconnected) return FALSE;

	if (psNET_ClientUpdate) 
		dwInterval = 1000 / psNET_ClientUpdate;

	if (psNET_Flags.test(NETFLAG_MINIMIZEUPDATES))	
		dwInterval = 1000;	// approx 3 times per second

	if (psNET_ClientUpdate && (dwTime - net_Time_LastUpdate) > dwInterval)
	{
		net_Time_LastUpdate = dwTime;
		return					TRUE;
	}
	else
		return					FALSE;
}

void	IPureClient::UpdateStatistic()
{
	// Query network statistic for this client
	DPN_CONNECTION_INFO	CI;
    std::memset(&CI,0,sizeof(CI));
	CI.dwSize			= sizeof(CI);
	HRESULT hr					= NET->GetConnectionInfo(&CI,0);
	if (FAILED(hr)) return;

	net_Statistic.Update(CI);
}

void IPureClient::Sync_Average	()
{
	//***** Analyze results
	s64		summary_delta	= 0;
	s32		size			= net_DeltaArray.size();
	u32*	I				= net_DeltaArray.begin();
	u32*  E					= I+size;
	for (; I!=E; I++)		summary_delta	+= *((int*)I);

	s64 frac				=	s64(summary_delta) % s64(size);
	if (frac<0)				frac=-frac;
	summary_delta			/=	s64(size);
	if (frac>s64(size/2))	summary_delta += (summary_delta<0)?-1:1;
	net_TimeDelta_Calculated=	s32(summary_delta);
	net_TimeDelta			=	(net_TimeDelta*5+net_TimeDelta_Calculated)/6;
}

void IPureClient::ClearStatistic()
{
	net_Statistic.Clear();
}

BOOL IPureClient::net_IsSyncronised()
{
	return net_Syncronised;
}
