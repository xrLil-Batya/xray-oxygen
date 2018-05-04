// new(0)
template <class T>
inline T* xr_new()
{
	T* ptr = (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T();
}
// new(...)
template <class T, class ... Args>
inline T* xr_new(const Args& ... args)
{
	T* ptr = (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr)T(args...);
}

template <bool _is_pm, typename T>
struct xr_special_free
{
	inline void operator()(T* &ptr)
	{
		void*	_real_ptr = dynamic_cast<void*>(ptr);
		ptr->~T();
		Memory.mem_free(_real_ptr);
	}
};

template <typename T>
struct xr_special_free<false, T>
{
	inline void operator()(T* &ptr)
	{
		ptr->~T();
		Memory.mem_free(ptr);
	}
};

template <class T>
inline void xr_delete(T* &ptr)
{
	if (ptr)
	{
		xr_special_free<std::is_polymorphic_v<T>, T>()(ptr);
		ptr = nullptr;
	}
}

template <class T>
inline void xr_delete(T* const &ptr)
{
	if (ptr)
	{
		T*& hacked_ptr = const_cast<T*&>(ptr);
		xr_special_free<std::is_polymorphic_v<T>, T>()(hacked_ptr);

		hacked_ptr = nullptr;
	}
}