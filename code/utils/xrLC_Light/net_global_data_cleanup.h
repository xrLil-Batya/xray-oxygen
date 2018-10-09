#ifndef _NET_GLOBAL_DATA_CLEANUP_
#define _NET_GLOBAL_DATA_CLEANUP_
#include "net_global_data.h"
#include "hxgrid/Interface/IAgent.h"
#include "hxgrid/Interface/hxgridinterface.h"
namespace lc_net
{
	class global_data_cleanup
	{
		
		u32						id_state	;				
		xr_vector<u32>			vec_cleanup	;
		xrCriticalSection		lock;
		friend					void  data_cleanup_callback( const char* dataDesc, IGenericStream** stream );
	public:
				global_data_cleanup		(  );

		template<e_net_globals data>
		void	set_cleanup					( u32 id )
		{
			xrCriticalSectionGuard guard(lock);
			if( vec_cleanup[data] == id )
			{
				//Leave.Leave(); commited 25.01.17
				// Leave.Unlock(); commited 25.01.17
				return;
			}
			++id_state;
			vec_cleanup[data] = id;
			//lock.unlock(); commited 25.01.17
		};
		template<e_net_globals data>
		u32 	get_cleanup					(  ) const
		{
			return vec_cleanup[data];
		};
		void	on_net_send		(IGenericStream* outStream );
		void	on_net_receive	( IAgent* agent, DWORD sessionId, IGenericStream* inStream );

	};
	global_data_cleanup &cleanup();
}

#endif