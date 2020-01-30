#pragma once
// resource itself, the base class for all derived resources
class	XRCORE_API	xr_resource	
{
public:
	enum			{RF_REGISTERED=1<<0 };
public:
	xr_atomic_s32	  dwReference;
	xr_resource()	: dwReference(0)		{ }
};

class	XRCORE_API	xr_resource_flagged	:	public xr_resource		
{
public:
	enum			{RF_REGISTERED=1<<0 };
public:
	u32				dwFlags;
	xr_resource_flagged()	: dwFlags(0)					{ }
};

class XRCORE_API xr_resource_named : public xr_resource_flagged	
{
public:
	shared_str		cName;

	const char *	set_name			( const char * name)	
	{
		cName		= name;
		return		*cName;
	}
	xr_resource_named()	: cName(0)		{ }
	~xr_resource_named()				{ }
};

// resptr_CORE
template <class T>
struct resptr_core
{
protected:
	typedef resptr_core			this_type;
	typedef resptr_core<T>		self;

	T* p_;
	// ref-counting
	void				_inc() 
	{ 
		if (0 == p_) return; 
		s32 NewRef = ++p_->dwReference;
		R_ASSERT(NewRef != 1);
	}

	void				_dec() 
	{ 
		if (0 == p_) return; 
		s32 NewRef = --p_->dwReference;
		if (0 >= NewRef) xr_delete(p_);
	}

public:
	ICF		void		_set(T* rhs) 
	{ 
		if (0 != rhs)
		{
			++rhs->dwReference;
		}
		_dec(); 
		p_ = rhs; 
	}
	ICF		void		_set(self const& rhs) { T* prhs = rhs._get(); _set(prhs); }
	ICF		T*			_get() const { return p_; }

			void				_clear() { p_ = 0; }
						// construction
						resptr_core		()												{	p_ = nullptr;						}
						resptr_core		(T * p, bool add_ref = true) 					
						{	
							p_ = p;	
							if (add_ref && p_ != nullptr)
							{
								p_->dwReference++; 
							}
						}
						resptr_core		(const self & rhs)								{	
							p_ = rhs.p_; 
							if (p_ != nullptr)
							{
								p_->dwReference++;		
							}
						}
						~resptr_core	()												{	_dec();						}

						// assignment
	self &				operator=		(const self & rhs)								{	_set(rhs);	return (self&)*this;	}

						// accessors
	T &					operator*() const												{	return *p_;	}
	T *					operator->() const												{	return p_;	}

						// unspecified bool type
						typedef T * (resptr_core::*unspecified_bool_type) () const;
						operator unspecified_bool_type () const							{	return p_ == 0? 0: &resptr_core::_get;	}
						bool operator!	() const										{	return p_ == 0;	}

						// fast swapping
	void				swap			(self & rhs)									{	T * tmp = p_; p_ = rhs.p_; rhs.p_ = tmp;	}
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr
template<class T, class U>	inline bool operator	==	(resptr_core<T> const & a, resptr_core<U> const & b)		{ return a._get() == b._get();						}
template<class T, class U>	inline bool operator	!=	(resptr_core<T> const & a, resptr_core<U> const & b)		{ return a._get() != b._get();						}
template<class T>			inline bool operator	==	(resptr_core<T> const & a, T * b)							{ return a._get() == b;								}
template<class T>			inline bool operator	!=	(resptr_core<T> const & a, T * b)							{ return a._get() != b;								}
template<class T>			inline bool operator	==	(T * a, resptr_core<T> const & b)							{ return a == b._get();								}
template<class T>			inline bool operator	!=	(T * a, resptr_core<T> const & b)							{ return a != b._get();								}
template<class T>			inline bool operator	<	(resptr_core<T> const & a, resptr_core<T> const & b)		{ return std::less<T *>()(a._get(), b._get());		}
template<class T>			inline bool operator	>	(resptr_core<T> const & a, resptr_core<T> const & b)		{ return std::less<T *>()(b._get(), a._get());		}

// externally visible swap
template<class T> void swap	(resptr_core<T> & lhs, resptr_core<T> & rhs)									{ lhs.swap(rhs);	}

// mem_fn support
template<class T> T * get_pointer(resptr_core<T> const & p)													{ return p.get();	}

// casting
template<class T, class U> resptr_core<T> static_pointer_cast(resptr_core<U> const & p)						{ return static_cast<T *>(p.get());				}
template<class T, class U> resptr_core<T> dynamic_pointer_cast(resptr_core<U> const & p)					{ return dynamic_cast<T *>(p.get());			}