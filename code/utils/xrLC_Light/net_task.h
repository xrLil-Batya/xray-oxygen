#pragma once
#include "light_execute.h"
#include "net_task_callback.h"

	class net_task:
		public net_task_callback
	{

	
		CDeflector *_D		;
		u32		_id			;
		light_execute _execute;
	public:
		void run				( );

	
		bool receive			( IGenericStream* inStream) ;
		bool send				( IGenericStream* outStream );
		
		net_task				( IAgent *agent, DWORD session);
		~net_task				( );
	};



