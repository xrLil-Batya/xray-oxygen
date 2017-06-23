#pragma once

#ifdef DEBUG
#	define	USE_MEMORY_MONITOR
#endif // DEBUG

#ifdef USE_MEMORY_MONITOR

namespace memory_monitor {
	XRCORE_API void flush_each_time	(const bool &value);
	XRCORE_API void monitor_alloc	(const void *pointer, const size_t &size, LPCSTR description);
	XRCORE_API void	monitor_free	(const void *pointer);
	XRCORE_API void	make_checkpoint	(LPCSTR name);
	extern XRCORE_API int counter;
	extern XRCORE_API int counter_alloc;
	extern XRCORE_API int counter_free;
}
#endif