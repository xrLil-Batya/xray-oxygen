#include "stdafx.h"
#include "dxerr/dxerr.h"
#include "NET_Common.h"
#include "net_server.h"
#include <functional>

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

ENGINE_API int		psNET_ServerUpdate	= 30;		// FPS
ENGINE_API int		psNET_ServerPending	= 3;

ENGINE_API ClientID BroadcastCID(0xffffffff);

IClient::IClient(CTimer* timer) : stats(timer), server(nullptr), dwTime_LastUpdate(0)
{
	flags.bLocal = FALSE;
	flags.bConnected = FALSE;
	flags.bReconnect = FALSE;
	flags.bVerified = TRUE;
}

void IClientStatistic::Update(DPN_CONNECTION_INFO& CI)
{
	u32 time_global		= TimeGlobal(device_timer);
	if (time_global-dwBaseTime >= 999)
	{
		dwBaseTime		= time_global;
		
		mps_recive		= CI.dwMessagesReceived - mps_receive_base;
		mps_receive_base= CI.dwMessagesReceived;

		u32	cur_msend	= CI.dwMessagesTransmittedHighPriority+CI.dwMessagesTransmittedNormalPriority+CI.dwMessagesTransmittedLowPriority;
		mps_send		= cur_msend - mps_send_base;
		mps_send_base	= cur_msend;

		dwBytesSendedPerSec		= dwBytesSended;
		dwBytesSended			= 0;
		dwBytesReceivedPerSec	= dwBytesReceived;
		dwBytesReceived			= 0;
	}
	ci_last	= CI;
}

// {0218FA8B-515B-4bf2-9A5F-2F079D1759F3}
static const GUID NET_GUID = 
{ 0x218fa8b, 0x515b, 0x4bf2, { 0x9a, 0x5f, 0x2f, 0x7, 0x9d, 0x17, 0x59, 0xf3 } };
// {8D3F9E5E-A3BD-475b-9E49-B0E77139143C}
static const GUID CLSID_NETWORKSIMULATOR_DP8SP_TCPIP =
{ 0x8d3f9e5e, 0xa3bd, 0x475b, { 0x9e, 0x49, 0xb0, 0xe7, 0x71, 0x39, 0x14, 0x3c } };
//------------------------------------------------------------------------------
void IClient::_SendTo_LL( const void* data, u32 size, u32 uflags, u32 timeout )
{
    R_ASSERT(server);
    server->IPureServer::SendTo_LL( ID, const_cast<void*>(data), size, uflags, timeout );
}
//------------------------------------------------------------------------------
IClient* IPureServer::ID_to_client(ClientID ID, bool ScanAll)
{
	if(ID.value())
	{
		IClient* ret_client = GetClientByID(ID);
		if (ret_client || !ScanAll)
			return ret_client;
	}
	return nullptr;
}

void
IPureServer::_Recieve( const void* data, u32 data_size, u32 param )
{
	if (data_size >= NET_PacketSizeLimit) {
		Msg		("! too large packet size[%d] received, DoS attack?", data_size);
		return;
	}

    NET_Packet packet(data, data_size);
    ClientID    id(param);

	csMessage.lock();
	//---------------------------------------
	u32	result = OnMessage( packet, id );
	//Msg("-S- Leaving from csMessages [%d]", currentThreadId);
	csMessage.unlock();
	
	if( result )		
	    SendBroadcast( id, packet, result );
}

//==============================================================================

IPureServer::IPureServer(CTimer* timer)
{
	device_timer			= timer;
	stats.clear				();
	stats.dwSendTime		= TimeGlobal(device_timer);
	SV_Client				= NULL;
	NET						= NULL;
	net_Address_device		= NULL;
#ifdef DEBUG
	sender_functor_invoked = false;
#endif
}

IPureServer::~IPureServer	()
{
	SV_Client = nullptr;
}

IPureServer::EConnect IPureServer::Connect(LPCSTR options, GameDescriptionData & game_descr)
{
	// Parse options
    string1024 session_name;
    xr_strcpy(session_name, options);
    if (strchr(session_name, '/'))	*strchr(session_name, '/') = 0;
    connect_options = options;
	return	ErrNoError;
}

void IPureServer::Disconnect	()
{
    if( NET )	NET->Close(0);
	
	// Release interfaces
    _RELEASE	(net_Address_device);
    _RELEASE	(NET);
}

void IServerStatistic::clear()
{
	bytes_out = bytes_out_real = 0;
	bytes_in = bytes_in_real = 0;

	dwBytesSended = 0;
	dwSendTime = 0;
	dwBytesPerSec = 0;
}

void IPureServer::Flush_Clients_Buffers()
{
	struct LocalSenderFunctor
	{
		static void FlushBuffer(IClient* client)
		{
			client->MultipacketSender::FlushSendBuffer(0);
		}
	};
	
	net_players.ForEachClientDo(LocalSenderFunctor::FlushBuffer);
}

void IPureServer::SendTo_Buf(ClientID id, void* data, u32 size, u32 dwFlags, u32 dwTimeout)
{
	IClient* tmp_client = net_players.GetFoundClient(
		ClientIdSearchPredicate(id));
	VERIFY(tmp_client);
	tmp_client->MultipacketSender::SendPacket(data, size, dwFlags, dwTimeout);
}


void IPureServer::SendTo_LL(ClientID ID/*DPNID ID*/, void* data, u32 size, u32 dwFlags, u32 dwTimeout)
{
	// send it
	DPN_BUFFER_DESC		desc;
	desc.dwBufferSize	= size;
	desc.pBufferData	= LPBYTE(data);

#ifdef _DEBUG
	u32 time_global		= TimeGlobal(device_timer);
	if (time_global - stats.dwSendTime >= 999)
	{
		stats.dwBytesPerSec = (stats.dwBytesPerSec*9 + stats.dwBytesSended)/10;
		stats.dwBytesSended = 0;
		stats.dwSendTime = time_global;
	};
	if ( ID.value() )
		stats.dwBytesSended += size;
#endif

	// verify
	VERIFY		(desc.dwBufferSize);
	VERIFY		(desc.pBufferData);

	DPNHANDLE	hAsync	= 0;
	HRESULT		_hr		= NET->SendTo(ID.value(), &desc,1, dwTimeout, 0, &hAsync, dwFlags | DPNSEND_COALESCE);

	if (SUCCEEDED(_hr) || (DPNERR_CONNECTIONLOST==_hr))	return;

	R_CHK		(_hr);

}

void IPureServer::SendTo(ClientID ID/*DPNID ID*/, NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	SendTo_LL( ID, P.B.data, (u32)P.B.count, dwFlags, dwTimeout );
}

void IPureServer::SendBroadcast_LL(ClientID exclude, void* data, u32 size, u32 dwFlags)
{
	struct ClientExcluderPredicate
	{
		ClientID id_to_exclude;
		ClientExcluderPredicate(ClientID exclude) : id_to_exclude(exclude) {}
		bool operator()(IClient* client)
		{
			if (client->ID == id_to_exclude)
				return false;
			if (!client->flags.bConnected)
				return false;
			return true;
		}
	};
	struct ClientSenderFunctor
	{
		IPureServer*	m_owner;
		void*			m_data;
		u32				m_size;
		u32				m_dwFlags;
		ClientSenderFunctor(IPureServer* owner, void* data, u32 size, u32 dwFlags) :
			m_owner(owner), m_data(data), m_size(size), m_dwFlags(dwFlags)
		{}
		void operator()(IClient* client)
		{
			m_owner->SendTo_LL(client->ID, m_data, m_size, m_dwFlags);			
		}
	};
	ClientSenderFunctor temp_functor(this, data, size, dwFlags);
	net_players.ForFoundClientsDo(ClientExcluderPredicate(exclude), temp_functor);
}

void	IPureServer::SendBroadcast(ClientID exclude, NET_Packet& P, u32 dwFlags)
{
	// Perform broadcasting
	SendBroadcast_LL( exclude, P.B.data, (u32)P.B.count, dwFlags );
}

void IPureServer::OnCL_Connected		(IClient* CL)
{
	Msg("* Player 0x%08x connected.\n",	CL->ID.value());
}
void IPureServer::OnCL_Disconnected		(IClient* CL)
{
	Msg("* Player 0x%08x disconnected.\n", CL->ID.value());
}

BOOL IPureServer::HasBandwidth(IClient* C)
{
	u32	dwTime = TimeGlobal(device_timer);
	u32	dwInterval = 0;

	UpdateClientStatistic(C);
	C->dwTime_LastUpdate = dwTime;
	dwInterval = 1000;
	return TRUE;
}

void IPureServer::UpdateClientStatistic(IClient* C)
{
	// Query network statistic for this client
	DPN_CONNECTION_INFO			CI;
	std::memset(&CI, 0, sizeof(CI));
	CI.dwSize = sizeof(CI);
	C->stats.Update(CI);
}

void	IPureServer::ClearStatistic	()
{
	stats.clear();
	struct StatsClearFunctor
	{
		static void Clear(IClient* client)
		{
			client->stats.Clear();
		}
	};
	net_players.ForEachClientDo(StatsClearFunctor::Clear);
};

bool IPureServer::DisconnectClient(IClient* C, LPCSTR Reason)
{
	if (!C) return false;

	HRESULT res = NET->DestroyClient(C->ID.value(), Reason, xr_strlen(Reason)+1, 0);
	CHK_DX(res);
	return true;
}