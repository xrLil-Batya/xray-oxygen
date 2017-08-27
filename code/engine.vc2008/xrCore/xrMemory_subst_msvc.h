// new(0)
template <class T>
IC	T*		xr_new()
{
#ifdef DEBUG_MEMORY_NAME
	T* ptr = (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
#else
	T* ptr = (T*)Memory.mem_alloc(sizeof(T));
#endif
	return new (ptr) T();
}
// new(...)
template <class T, class ... Args>
IC    T*        xr_new(const Args& ... args)
{
#ifdef DEBUG_MEMORY_NAME
	T* ptr = (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
#else
	T* ptr = (T*)Memory.mem_alloc(sizeof(T));
#endif
	return new (ptr)T(args...);
}


template <bool _is_pm, typename T>
struct xr_special_free
{
	IC void operator()(T* &ptr)
	{
		void*	_real_ptr = dynamic_cast<void*>(ptr);
		ptr->~T();
		Memory.mem_free(_real_ptr);
	}
};

template <typename T>
struct xr_special_free<false, T>
{
	IC void operator()(T* &ptr)
	{
		ptr->~T();
		Memory.mem_free(ptr);
	}
};

template <class T>
IC	void	xr_delete(T* &ptr)
{
	if (ptr)
	{
		xr_special_free<std::is_polymorphic_v<T>, T>()(ptr);
		ptr = nullptr;
	}
}

template <class T>
IC	void	xr_delete(T* const &ptr)
{
    if (ptr)
    {
        T*& hacked_ptr = const_cast<T*&>(ptr);
        xr_special_free<std::is_polymorphic_v<T>, T> spec;
        spec(hacked_ptr);

        //const_cast<T*&>(ptr) = nullptr;
    }
}

#ifdef DEBUG_MEMORY_MANAGER
void XRCORE_API mem_alloc_gather_stats(const bool &value);
void XRCORE_API mem_alloc_gather_stats_frequency(const float &value);
void XRCORE_API mem_alloc_show_stats();
void XRCORE_API mem_alloc_clear_stats();
#endif // DEBUG_MEMORY_MANAGER