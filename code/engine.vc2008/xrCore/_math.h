#pragma once
#include "cpuid.h"

namespace FPU 
{
	XRCORE_API void	 m24	(void);
	XRCORE_API void	 m24r	(void);	
	XRCORE_API void	 m53	(void);	
	XRCORE_API void	 m53r	(void);	
	XRCORE_API void	 m64	(void);	
	XRCORE_API void	 m64r	(void);	
};

namespace CPU 
{
	XRCORE_API extern u64				clk_per_second		;
	XRCORE_API extern u64				qpc_freq			;
	XRCORE_API extern u32				qpc_counter			;

	XRCORE_API extern	processor_info	ID					;
	XRCORE_API extern	u64				QPC	()				;

#ifdef M_VISUAL
		IC u64	GetCLK(void){
			return __rdtsc();
		}
#endif
};

XRCORE_API void _initialize_cpu();
XRCORE_API void set_current_thread_name(const char* threadName);
XRCORE_API void set_thread_name(const char* threadName, std::thread& thread);