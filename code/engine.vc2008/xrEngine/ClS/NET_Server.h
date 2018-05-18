#pragma once
#include "net_shared.h"
#include "NET_PlayersMonitor.h"

// -----------------------------------------------------

class IPureServer;

class ENGINE_API IClient
{
public:
	struct Flags
	{
		u32		bConnected	: 1;
	};

                        IClient( CTimer* timer );
	virtual             ~IClient() = default;

	ClientID			ID;
	shared_str			name;

	Flags				flags;	// local/host/normal
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

class ENGINE_API IPureServer
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

	PlayersMonitor			net_players;
	IClient*				SV_Client;

	// 
	std::recursive_mutex		csMessage;
	
	// Statistic
	CTimer*					device_timer;

	IClient*				ID_to_client		(ClientID ID, bool ScanAll = false);
public:
							IPureServer (CTimer* timer);
	virtual					~IPureServer		();
	
	virtual EConnect		Connect				(LPCSTR session_name);

	// extended functionality
	virtual u32				OnMessage			(NET_Packet& P, ClientID sender) = 0;

	virtual void			client_Destroy		(IClient* C)	= 0;			// destroy client info

	IClient*				GetServerClient		()			{ return SV_Client; };

	//WARNING! very bad method :(
	IClient*				GetClientByID	(ClientID clientId)					{return net_players.GetFoundClient(ClientIdSearchPredicate(clientId));};


	const shared_str&		GetConnectOptions	() const {return connect_options;}
};

