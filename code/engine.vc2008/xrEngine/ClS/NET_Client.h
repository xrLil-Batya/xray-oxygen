#pragma once

#include "net_shared.h"
#include "NET_Common.h"
//==============================================================================
class ENGINE_API INetQueue
{
	std::recursive_mutex cs;
	xr_deque<NET_Packet*>	ready;
	xr_vector<NET_Packet*>	unused;
public:
	INetQueue();
	~INetQueue();

	NET_Packet*			Create	();
	NET_Packet*			Create	(const NET_Packet& _other);
	NET_Packet*			Retreive();
	void				Release	();
	inline void			Lock	() { cs.lock(); };
	inline void			Unlock	() { cs.unlock(); };
};
//==============================================================================
class ENGINE_API IPureClient
{
protected:
	GameDescriptionData		m_game_description;
	CTimer*					device_timer;

protected:
	INetQueue				net_Queue;

	void					SetClientID		(ClientID const & local_client) { net_ClientID = local_client; };
	
public:
	IPureClient				(CTimer* tm);
	virtual ~IPureClient	();
	
	void					Disconnect				();

	IC GameDescriptionData const & get_net_DescriptionData() const { return m_game_description; }

	// receive
	IC void							StartProcessQueue		()	{ net_Queue.Lock(); }; // WARNING ! after Start mast be End !!! <-
	IC virtual	NET_Packet*			net_msg_Retreive		()	{ return net_Queue.Retreive();	};//							|
	IC void							net_msg_Release			()	{ net_Queue.Release();			};//							|
	IC void							EndProcessQueue			()	{ net_Queue.Unlock();			};//							<-

	virtual void			OnMessage				(void* data, u32 size);
	ClientID const &		GetClientID				() { return net_ClientID; };
	
	// time management
	IC u32					timeServer				()	{ return device_timer->GetElapsed_ms(); }
	IC u32					timeServer_Async		()	{ return device_timer->GetElapsed_ms(); }

private:
	ClientID				net_ClientID;
};

