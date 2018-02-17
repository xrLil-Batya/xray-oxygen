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
	players_collection_t		net_Players_disconnected;
	bool						now_iterating_in_net_players;
	bool						now_iterating_in_net_players_disconn;
	DWORD						iterator_thread_id;
public:
	PlayersMonitor()
	{
	  now_iterating_in_net_players			= false;
	  now_iterating_in_net_players_disconn	= false;
	  iterator_thread_id					= 0;
	}
#ifdef DEBUG
	bool IsCurrentThreadIteratingOnClients() const
	{
		if (now_iterating_in_net_players || now_iterating_in_net_players_disconn)
		{
			if (iterator_thread_id == GetCurrentThreadId())
			{
				return true;
			}
		}
		return false;
	}
#endif
	template<typename ActionFunctor>
	void ForEachClientDo					(ActionFunctor & functor)
	{
		csPlayers.lock();
		now_iterating_in_net_players	=	true;
#ifdef DEBUG
		iterator_thread_id = GetCurrentThreadId();
#endif
		for (IClient* it : net_Players)
		{
			VERIFY2(it != 0, "IClient ptr is NULL");
			functor(it);
		}
		now_iterating_in_net_players	=	false;
		csPlayers.unlock();
	}
	void ForEachClientDo(fastdelegate::FastDelegate1<IClient*, void> & fast_delegate)
	{
        std::lock_guard<decltype(csPlayers)> lock(csPlayers);
		now_iterating_in_net_players	=	true;
#ifdef DEBUG
		iterator_thread_id = GetCurrentThreadId();
#endif
		for (IClient* it: net_Players)
		{
			VERIFY2(it != 0, "IClient ptr is NULL");
			fast_delegate(it);
		}
		now_iterating_in_net_players = false;
	}
	template<typename SearchPredicate, typename ActionFunctor>
	u32	ForFoundClientsDo(SearchPredicate const & predicate,	ActionFunctor & functor)
	{
		u32 ret_count = 0;
		csPlayers.lock();
		now_iterating_in_net_players = true;
#ifdef DEBUG
		iterator_thread_id = GetCurrentThreadId();
#endif
		players_collection_t::iterator players_endi = net_Players.end();
		players_collection_t::iterator temp_iter = std::find_if(net_Players.begin(), players_endi, predicate);
		
		while (temp_iter != players_endi)
		{
			VERIFY2(*temp_iter != 0, "IClient ptr is NULL");
			functor(*temp_iter);
			temp_iter = std::find_if(++temp_iter, players_endi, predicate);
		}
		now_iterating_in_net_players = false;
		csPlayers.unlock();
		return ret_count;
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

	u32 ClientsCount()
	{
        std::lock_guard<decltype(csPlayers)> lock(csPlayers);
		return u32(net_Players.size());
	}
}; //class PlayersMonitor