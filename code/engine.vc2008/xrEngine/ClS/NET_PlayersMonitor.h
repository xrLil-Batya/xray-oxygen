#pragma once
#include "net_shared.h"
#include "NET_Common.h"
#include "../xrCore/fastdelegate.h"
#include <limits> 

class IClient;

class PlayersMonitor
{
private:
	typedef xr_vector<IClient*>	players_collection_t;
	std::recursive_mutex			csPlayers;
	players_collection_t		net_Players;
	bool						now_iterating_in_net_players;
	DWORD						iterator_thread_id;
public:
	PlayersMonitor()
	{
	  now_iterating_in_net_players			= false;
	  iterator_thread_id					= 0;
	}
	
	template<typename SearchPredicate>
	IClient*	FindAndEraseClient				(SearchPredicate const & predicate)
	{
        std::lock_guard<decltype(csPlayers)> lock(csPlayers);
		VERIFY(!now_iterating_in_net_players);
		now_iterating_in_net_players = true;
#ifdef DEBUG
		iterator_thread_id = GetCurrentThreadId();
#endif
		players_collection_t::iterator client_iter = std::find_if(
			net_Players.begin(),
			net_Players.end(),
			predicate);
		IClient* ret_client = nullptr;
		if (client_iter != net_Players.end())
		{
			ret_client = *client_iter;
			net_Players.erase(client_iter);
		}
		now_iterating_in_net_players = false;
		return ret_client;
	}
	template<typename SearchPredicate>
	IClient* GetFoundClient(SearchPredicate const & predicate)
	{
		csPlayers.lock();
		players_collection_t::iterator client_iter = std::find_if(net_Players.begin(), net_Players.end(), predicate);
		IClient* ret_client = nullptr;
		if (client_iter != net_Players.end())
		{
			ret_client = *client_iter;
		}
		csPlayers.unlock();
		return ret_client;
	}
	void AddNewClient(IClient* new_client)
	{
        std::lock_guard<decltype(csPlayers)> lock(csPlayers);
		VERIFY(!now_iterating_in_net_players);
		net_Players.push_back(new_client);
	}
}; //class PlayersMonitor