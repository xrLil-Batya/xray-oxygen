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

class ENGINE_API IClient
{
public:
	struct Flags
	{
		u32		bLocal		: 1;
		u32		bConnected	: 1;
	};

                        IClient( CTimer* timer );
	virtual             ~IClient() = default;

	IClientStatistic	stats;

	ClientID			ID;
	shared_str			name;
	shared_str			pass;

	Flags				flags;	// local/host/normal
	u32					dwTime_LastUpdate;
	
    IPureServer*        server;
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
	
	virtual IClient*		new_client			(SClientConnectData* cl_data) = 0;
public:
							IPureServer (CTimer* timer);
	virtual					~IPureServer		();
	
	virtual EConnect		Connect				(LPCSTR session_name);

	// extended functionality
	virtual u32				OnMessage			(NET_Packet& P, ClientID sender) = 0;

	virtual IClient*		client_Create		()				= 0;			// create client info
	virtual void			client_Destroy		(IClient* C)	= 0;			// destroy client info

	IClient*				GetServerClient		()			{ return SV_Client; };
	template<typename SearchPredicate>
	IClient*				FindClient		(SearchPredicate const & predicate) { return net_players.GetFoundClient(predicate); }
	template<typename ActionFunctor>
	void					ForEachClientDo	(ActionFunctor & action)			{ net_players.ForEachClientDo(action); }
	template<typename SenderFunctor>
	void					ForEachClientDoSender(SenderFunctor & action)		{ 
                                                                                    std::lock_guard<decltype(csMessage)> lock(csMessage);
																					net_players.ForEachClientDo(action);
																				}
	//WARNING! very bad method :(
	IClient*				GetClientByID	(ClientID clientId)					{return net_players.GetFoundClient(ClientIdSearchPredicate(clientId));};


	const shared_str&		GetConnectOptions	() const {return connect_options;}
};

