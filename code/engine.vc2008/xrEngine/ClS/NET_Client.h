#pragma once

//==============================================================================
class ENGINE_API INetQueue
{
	xrCriticalSection cs;
	xr_deque<NET_Packet*>	ready;
	xr_vector<NET_Packet*>	unused;
public:
	INetQueue();
	~INetQueue();

	NET_Packet*			Create	();
	NET_Packet*			Create	(const NET_Packet& _other);
	NET_Packet*			Retreive();
	void				Release	();
	inline void			Lock	() { cs.Enter(); };
	inline void			Unlock	() { cs.Leave(); };
};
//==============================================================================
class ENGINE_API IPureClient
{
protected:
	CTimer*					device_timer;

protected:
	INetQueue				net_Queue;
	
public:
	IPureClient				(CTimer* tm);
	virtual ~IPureClient	();
	
	void					Disconnect				();

	// receive
	IC void							StartProcessQueue		()	{ net_Queue.Lock(); }; // WARNING ! after Start mast be End !!! <-
	IC virtual	NET_Packet*			net_msg_Retreive		()	{ return net_Queue.Retreive();	};//							|
	IC void							net_msg_Release			()	{ net_Queue.Release();			};//							|
	IC void							EndProcessQueue			()	{ net_Queue.Unlock();			};//							<-

	virtual void			OnMessage				(void* data, u32 size);
	
	// time management
	IC u32					timeServer				()	{ return device_timer->GetElapsed_ms(); }
	IC u32					timeServer_Async		()	{ return device_timer->GetElapsed_ms(); }
};

