//////////////////////////////////////////////////////////////////////
// Desc    : X-Ray Oxygen memory allocators
// Authors : ForserX, Giperion, GSC
//////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>

// Fast alloc for fast manipulation of the memory
template <typename T>
class XAllocPtr
{
public:
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using pointer = T * ;
	using const_pointer = const T*;
	using reference = T & ;
	using const_reference = const T&;
	using value_type = T;

	pointer address(reference ref) const { return &ref; }
	const_pointer address(const_reference ref) const { return &ref; }

	XAllocPtr() = default;
	XAllocPtr(const XAllocPtr<T>&) = default;

	template <class Other>
	XAllocPtr(const XAllocPtr<Other>&) {};

	template <class Other>
	XAllocPtr& operator=(const XAllocPtr<Other>&) 
	{
		return *this;
	}
#pragma warning(push)
#pragma warning(disable: 4267)
	pointer allocate(const size_type n, const void* p = nullptr) const
	{
		size_t MemoryRequired = sizeof(T) * n;
		return (pointer)malloc(MemoryRequired);
	}
#pragma warning(pop)

	void deallocate(pointer p, const size_type) const { free(p); }

	void deallocate(void* p, const size_type) const { free(p); }

	void construct(pointer p, const T& _Val) { new (p) T(_Val); }

	void destroy(pointer p) { p->~T(); }
};

// Default allocator for all containers  
template <typename T>
class xalloc 
{
public:
	using difference_type = ptrdiff_t;
	using pointer = T * ;
	using const_pointer = const T*;
	using reference = T & ;
	using const_reference = const T&;
	using value_type = T;

	template <class Other>
	struct rebind
	{
		using other = xalloc<Other>;
	};

	pointer address(reference ref) const { return &ref; }
	const_pointer address(const_reference ref) const { return &ref; }

	xalloc() = default;
	xalloc(const xalloc<T>&) = default;

	template <class Other>
	xalloc(const xalloc<Other>&) {}

	template <class Other>
	xalloc& operator=(const xalloc<Other>&) 
	{
		return *this;
	}
#pragma warning(push)
#pragma warning(disable: 4267)
	pointer allocate(const size_t n, const void* p = nullptr) const { return xr_alloc<T>(n); }
#pragma warning(pop)

	void deallocate(pointer p, const size_t size_type) const 
	{ 
		xr_free(p); 
	}

	void deallocate(void* p, const size_t size_type) const
	{ 
		xr_free(p); 
	}

	void construct(pointer p, const T& _Val) { new (p) T(_Val); }

	void destroy(pointer p) { p->~T(); }

	size_t max_size() const 
	{
		const auto count = std::numeric_limits<size_t>::max() / sizeof(T);
		return 0 < count ? count : 1;
	}
};

namespace std
{
	template <class Tp1, class Tp2>
	xalloc<Tp2>& __stl_alloc_rebind(xalloc<Tp1>& a, const Tp2*) 
	{
		return (xalloc<Tp2>&)(a);
	}

	template <class Tp1, class Tp2>
	xalloc<Tp2> __stl_alloc_create(xalloc<Tp1>&, const Tp2*)
	{
		return xalloc<Tp2>();
	}

} // namespace std

#if 0
// for list container ONLY!!
// do NOT use in std::vector or any container that allowing sequence access to elements
template <typename T>
class XAllocLazy
{
	using difference_type = ptrdiff_t;
	using pointer = T * ;
	using const_pointer = const T*;
	using reference = T & ;
	using const_reference = const T&;
	using value_type = T;

	template<typename Elem = T>
	struct AllocatorNode
	{
		bool isDeleted;
		Elem theElement;
	};

	pointer address(reference ref) const { return &ref; }
	const_pointer address(const_reference ref) const { return &ref; }

	template <class Other>
	XAllocLazy(const XAllocLazy<Other>&) {}

	template <class Other>
	XAllocLazy& operator=(const XAllocLazy<Other>&) 
	{
		return *this;
	}
#pragma warning(push)
#pragma warning(disable: 4267)
	pointer allocate(const size_type n, const void* p = nullptr) const
	{
		// we should allocate very rare. Most of the time - use existent allocated space

		if (pBuffer == nullptr)
		{
			AllocatedNum = n * 5;
			Size = n;
			pBuffer = xr_alloc<AllocatorNode<T>>(n * 5);
		}
		else
		{
			// check if we don't have enough space
			if (Size + n > AllocatedNum)
			{
				AllocatedNum = AllocatedNum + n + 5;
				pBuffer = xr_realloc(pBuffer, AllocatedNum * sizeof(AllocatorNode<T>));
			}
			Size += n;
		}

		return (pointer)pBuffer[Size - n];
	}
#pragma warning(pop)

	void internalDeallocate(void* p)
	{
		size_t InternalIndex = (p - pBuffer) / sizeof(T);

	}

	void deallocate(pointer p, const size_type) const
	{
		xr_free(p);
	}

	void deallocate(void* p, const size_type) const
	{
		xr_free(p);
	}

	void construct(pointer p, const T& _Val) { new (p) T(_Val); }

	void destroy(pointer p) { p->~T(); }

	size_t max_size() const
	{
		const unsigned long long count = std::numeric_limits<size_t>::max() / sizeof(T);
		return 0 < count ? count : 1;
	}

	size_t AllocatedNum;
	size_t Size;
	void* pBuffer;
};

#endif

// Render allocator (need impl)
#define render_alloc xalloc

// Helper struct
struct xr_allocator
{
	template <typename T>
	struct helper
	{
		using result = xalloc<T>;
	};

	static void* alloc(const size_t n) { return xr_malloc(n); }

	template <typename T>
	static void dealloc(T*& p)
	{
		xr_free(p);
	}
};

template <class T, class Other>
bool operator==(const xalloc<T>&, const xalloc<Other>&) 
{
	return true;
}

template <class T, class Other>
bool operator!=(const xalloc<T>&, const xalloc<Other>&) 
{
	return false;
}