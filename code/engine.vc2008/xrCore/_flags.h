#ifndef __FLAGS_H__
#define __FLAGS_H__

template <class T>
struct _flags {
public:
	typedef T			TYPE;
	typedef _flags<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T 	flags;

    IC	TYPE	get		()									const	{ return flags;}
    IC	SelfRef	zero	()											{ flags=T(0);	return *this;			}
    IC	SelfRef	one		()											{ flags=T(-1);	return *this;			}
    IC	SelfRef	invert	()											{ flags	=	~flags;		return *this;	}
    IC	SelfRef	invert	(const Self& f)								{ flags	=	~f.flags;	return *this;	}
    IC	SelfRef	invert	(const T mask)								{ flags ^=	mask;		return *this;	}
	IC	SelfRef	assign	(const Self& f)								{ flags =	f.flags;	return *this;	}
	IC	SelfRef	assign	(const T mask)								{ flags	=	mask;		return *this;	}
	IC	SelfRef	set		(const T mask,	bool value)					{ if (value) flags|=mask; else flags&=~mask; return *this; }
	IC 	bool	is		(const T mask)						const	{ return mask==(flags&mask);			}
	IC  bool    bitTest (const int bitNum)					const	{ u64 tempFlag = flags; return _bittest64((LONG64*)&tempFlag, bitNum); }
	IC 	bool	is_any	(const T mask)						const	{ return !!(flags&mask);				}
	IC 	bool	test	(const T mask)						const	{ return !!(flags&mask);				}
	IC 	SelfRef	or		(const T mask)								{ flags|=mask;			return *this;	}
	IC 	SelfRef	or		(const Self& f, const T mask) 				{ flags=f.flags|mask;	return *this;	}
	IC 	SelfRef	and		(const T mask)								{ flags&=mask;			return *this;	}
	IC 	SelfRef	and		(const Self& f, const T mask) 				{ flags=f.flags&mask;	return *this;	}
	IC 	bool	equal	(const Self& f) 			  		const	{ return flags==f.flags;}
	IC 	bool	equal	(const Self& f, const T mask) 		const	{ return (flags&mask)==(f.flags&mask);}
};

typedef _flags<u8>	Flags8;		typedef _flags<u8>	flags8;		
typedef _flags<u16>	Flags16;	typedef _flags<u16>	flags16;
typedef _flags<u32>	Flags32;	typedef _flags<u32>	flags32;
typedef _flags<u64>	Flags64;	typedef _flags<u64>	flags64;

#endif //__FLAGS_H__
