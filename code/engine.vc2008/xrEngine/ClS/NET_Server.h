#pragma once

#include "net_shared.h"
#include "NET_Common.h"
#include "NET_PlayersMonitor.h"

struct SClientConnectData
{
	ClientID		clientID;
	string64		name;
	string64		pass;
	u32				process_id;

	SClientConnectData()
	{
		name[0] = pass[0] = 0;
		process_id = 0;
	}
};

// -----------------------------------------------------

class IPureServer;

class ENGINE_API 
IClient		: public MultipacketSender
{
public:
	struct Flags
	{
		u32		bLocal		: 1;
		u32		bConnected	: 1;
		u32		bReconnect	: 1;
		u32		bVerified	: 1;
	};

                        IClient( CTimer* timer );
	virtual             ~IClient() = default;

	IClientStatistic	stats;

	ClientID			ID;
	string128			m_guid;
	shared_str			name;
	shared_str			pass;

	Flags				flags;	// local/host/normal
	u32					dwTime_LastUpdate;
	
    IPureServer*        server;

private:

    virtual void    _SendTo_LL( const void* data, u32 size, u32 flags, u32 timeout );
};


IC bool operator== (IClient const* pClient, ClientID const& ID) { return pClient->ID == ID; }

//==============================================================================
struct ClientIdSearchPredicate
{
	ClientID clientId;
	ClientIdSearchPredicate(ClientID clientIdToSearch) :
		clientId(clientIdToSearch)
	{
	}
	inline bool operator()(IClient* client) const
	{
		return client->ID == clientId;
	}
};


class CServerInfo;

class ENGINE_API IPureServer: private MultipacketReciever
{
public:
	enum EConnect
	{
		ErrConnect,
		ErrMax,
		ErrNoError = ErrMax,
	};
protected:
	shared_str				connect_options;
	IDirectPlay8Server*		NET;
	IDirectPlay8Address*	net_Address_device;

	PlayersMonitor			net_players;
	IClient*				SV_Client;

	// 
	std::recursive_mutex		csMessage;
	
	// Statistic
	CTimer*					device_timer;

	IClient*				ID_to_client		(ClientID ID, bool ScanAll = false);
	
	virtual IClient*		new_client			(SClientConnectData* cl_data) = 0;
public:
							IPureServer (CTimer* timer);
	virtual					~IPureServer		();
	
	virtual EConnect		Connect				(LPCSTR session_name, GameDescriptionData & game_descr);
	virtual void			Disconnect			();

	// send
	virtual void			SendTo_LL			(ClientID ID, void* data, u32 size, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	virtual void			SendTo_Buf			(ClientID ID, void* data, u32 size, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	virtual void			Flush_Clients_Buffers	();

	void					SendTo				(ClientID ID, NET_Packet& P, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	void					SendBroadcast_LL	(ClientID exclude, void* data, u32 size, u32 dwFlags=DPNSEND_GUARANTEED);
	virtual void			SendBroadcast		(ClientID exclude, NET_Packet& P, u32 dwFlags=DPNSEND_GUARANTEED);

	// extended functionality
	virtual u32				OnMessage			(NET_Packet& P, ClientID sender) = 0;
	virtual void			OnCL_Connected		(IClient* C);
	virtual void			OnCL_Disconnected	(IClient* C);

	virtual IClient*		client_Create		()				= 0;			// create client info
	virtual void			client_Destroy		(IClient* C)	= 0;			// destroy client info

	virtual bool			DisconnectClient		(IClient* C, LPCSTR Reason);

	u32						GetClientsCount		()			{ return net_players.ClientsCount(); };
	IClient*				GetServerClient		()			{ return SV_Client; };
	template<typename SearchPredicate>
	IClient*				FindClient		(SearchPredicate const & predicate) { return net_players.GetFoundClient(predicate); }
	template<typename ActionFunctor>
	void					ForEachClientDo	(ActionFunctor & action)			{ net_players.ForEachClientDo(action); }
	template<typename SenderFunctor>
	void					ForEachClientDoSender(SenderFunctor & action)		{ 
                                                                                    std::lock_guard<decltype(csMessage)> lock(csMessage);
#ifdef DEBUG
																					sender_functor_invoked = true;
#endif //#ifdef DEBUG
																					net_players.ForEachClientDo(action);
#ifdef DEBUG
																					sender_functor_invoked = false;
#endif //#ifdef DEBUG
																				}
#ifdef DEBUG
	bool					IsPlayersMonitorLockedByMe()	const				{ return net_players.IsCurrentThreadIteratingOnClients() && !sender_functor_invoked; };
#endif
	//WARNING! very bad method :(
	IClient*				GetClientByID	(ClientID clientId)					{return net_players.GetFoundClient(ClientIdSearchPredicate(clientId));};


	const shared_str&		GetConnectOptions	() const {return connect_options;}
};

