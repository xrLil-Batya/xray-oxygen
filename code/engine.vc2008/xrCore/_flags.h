#ifndef __FLAGS_H__
#define __FLAGS_H__

template <class T>
struct _flags
{
public:
	typedef T TYPE;
	typedef _flags<T>	Self;
	typedef Self& SelfRef;
	typedef const Self&	SelfCRef;
public:
	T flags;

	inline	TYPE	get() const { return flags; }
	inline	SelfRef	zero() { flags = T(0); return *this; }
	inline	SelfRef	one() { flags = T(-1); return *this; }
	inline	SelfRef	invert() { flags = ~flags; return *this; }
	inline	SelfRef	invert(const Self& f) { flags = ~f.flags; return *this; }
	inline	SelfRef	invert(const T mask) { flags ^= mask; return *this; }
	inline	SelfRef	assign(const Self& f) { flags = f.flags; return *this; }
	inline	SelfRef	assign(const T mask) { flags = mask; return *this; }
	inline	SelfRef	set(const T mask, bool value) { if (value) flags |= mask; else flags &= ~mask; return *this; }
	inline 	bool is(const T mask) const { return mask == (flags&mask); }
	inline 	bool is_any(const T mask) const { return !!(flags&mask); }
	inline 	bool test(const T mask) const { return !!(flags&mask); }
	inline 	SelfRef or (const T mask) { flags |= mask; return *this; }
	inline 	SelfRef or (const Self& f, const T mask) { flags = f.flags | mask; return *this; }
	inline 	SelfRef and (const T mask) { flags &= mask; return *this; }
	inline 	SelfRef and (const Self& f, const T mask) { flags = f.flags&mask; return *this; }
	inline 	bool equal(const Self& f) const { return flags == f.flags; }
	inline 	bool equal(const Self& f, const T mask) const { return (flags&mask) == (f.flags&mask); }
};

typedef _flags<u8>	Flags8;
typedef _flags<u8>	flags8;
typedef _flags<u16>	Flags16;
typedef _flags<u16>	flags16;
typedef _flags<u32>	Flags32;
typedef _flags<u32>	flags32;
typedef _flags<u64>	Flags64;
typedef _flags<u64>	flags64;

#endif //__FLAGS_H__
