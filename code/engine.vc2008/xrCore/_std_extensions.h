#pragma once
#ifdef abs
#undef abs
#endif

#ifdef _MIN
#undef _MIN
#endif

#ifdef _MAX
#undef _MAX
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include "string_concatenations.h"
//#include <charconv>

// конвертирует строку в число, в случае ошибки возвращает 0
// (в отличии от atoi, у которого местами есть неопределённое поведение)
// UPDATE: charconv is not available on Xbox One (at least in that SDK that we using)
// so we use good old atoi

inline int atoi_17(const std::string_view str)
{
	string64 tempStr = {0};
	VERIFY(str.size() < 64);
	memcpy(tempStr, str.data(), str.size());
	return atoi(tempStr);
}


// token type definition
struct XRCORE_API xr_token
{
    xr_token() : name(nullptr), id(-1) {}
    xr_token(const char* _name, const int _id) : name(_name), id(_id) {}
	const char*	name;
	int 	id;
};

IC const char* get_token_name(xr_token* tokens, int key)
{
    for (int k=0; tokens[k].name; k++)
    	if (key==tokens[k].id) return tokens[k].name;
    return "";
}

IC int get_token_id(xr_token* tokens, const char* key)
{
    for (int k=0; tokens[k].name; k++)
    	if ( stricmp(tokens[k].name,key)==0 ) 
			return tokens[k].id;
    return -1;
}

struct XRCORE_API xr_token2
{
	const char*	name;
	const char*	info;
	int 	id;
};

// generic
template <class T>
IC T		_sqr	(T a) noexcept { return a*a; }

template <class T>
IC T		saturate(T a)			{ return std::clamp(a, (T)0, (T)1); }

template <class T>
IC T		lerp	(T v0, T v1, T t) { return fma(t, v1, fma(-t, v0, v0)); }

// float
IC float	_abs	(float x) noexcept { return fabsf(x); }
IC float	_sqrt	(float x) noexcept { return sqrtf(x); }
IC float	_sin	(float x) noexcept { return sinf(x);  }
IC float	_cos	(float x) noexcept { return cosf(x);  }
IC float    _log    (float x) noexcept { return logf(x);  }
IC BOOL		_valid	(const float x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( –INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls			= _fpclass		(double(x));
	if (cls&(_FPCLASS_SNAN+_FPCLASS_QNAN+_FPCLASS_NINF+_FPCLASS_PINF+_FPCLASS_ND+_FPCLASS_PD))	
		return	false;	

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero 
	_FPCLASS_NZ Negative zero ( – 0) 
	_FPCLASS_PZ Positive 0 (+0) 
	_FPCLASS_PN Positive normalized non-zero 
	*/
	return		true;
}


// double
IC double	_abs	(double x) noexcept { return fabs(x); }
IC double	_sqrt	(double x) noexcept { return sqrt(x); }
IC double	_sin	(double x) noexcept { return sin(x); }
IC double	_cos	(double x) noexcept { return cos(x); }
IC double    _log	(double x) noexcept { return log(x); }
IC BOOL		_valid	(const double x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( –INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls			= _fpclass		(x);
	if (cls&(_FPCLASS_SNAN+_FPCLASS_QNAN+_FPCLASS_NINF+_FPCLASS_PINF+_FPCLASS_ND+_FPCLASS_PD))	
		return false;	

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero 
	_FPCLASS_NZ Negative zero ( – 0) 
	_FPCLASS_PZ Positive 0 (+0) 
	_FPCLASS_PN Positive normalized non-zero 
	*/
	return		true;
}

// int8
IC s8		_abs	(s8  x)	 noexcept { return (x>=0)? x : s8(-x); }

// unsigned int8
IC u8		_abs	(u8 x)	 noexcept { return x; }

// int16
IC s16		_abs	(s16 x)	 noexcept { return (x>=0)? x : s16(-x); }

// unsigned int16
IC u16		_abs	(u16 x)	 noexcept { return x; }

// int32
IC s32		_abs	(s32 x)	 noexcept { return (x>=0)? x : s32(-x); }

// int64
IC s64		_abs	(s64 x)	 noexcept { return (x>=0)? x : s64(-x); }

IC u32							xr_strlen				( const char* S );

// string management

// return pointer to ".ext"
IC char*						strext					( const char* S )
{	return (char*) strrchr(S,'.');	}

IC u32							xr_strlen				( const char* S )
{	return (u32)strlen(S);			}

IC char*						xr_strlwr				(char* S)
{	return strlwr(S);				}

IC int							xr_strcmp				( const char* S1, const char* S2 )
{	return (int)strcmp(S1,S2);  }

#ifndef  _EDITOR
#ifndef MASTER_GOLD

inline errno_t xr_strcpy		( char* destination, size_t const destination_size, const char* source )
{
	return						strcpy_s( destination, destination_size, source );
}

inline errno_t xr_strcat		( char* destination, size_t const buffer_size, const char* source )
{
	return						strcat_s( destination, buffer_size, source );
}

inline int xr_sprintf	( char* destination, size_t const buffer_size, const char* format_string, ... )
{
	va_list args;
	va_start					( args, format_string);
	return						vsprintf_s( destination, buffer_size, format_string, args );
}
template <int count>
inline int xr_sprintf	( char (&destination)[count], const char* format_string, ... )
{
	va_list args;
	va_start					( args, format_string);
	return						vsprintf_s( destination, count, format_string, args );
}

#else // #ifndef MASTER_GOLD

inline errno_t xr_strcpy	( char* destination, size_t const destination_size, const char* source )
{
	return						strncpy_s( destination, destination_size, source, destination_size );
}

inline errno_t xr_strcat		( char* destination, size_t const buffer_size, const char* source )
{
	size_t const destination_length	= xr_strlen(destination);
	char* i						= destination + destination_length;
	char* const e				= destination + buffer_size - 1;
	if ( i > e )
		return					0;

	for ( const char* j = source; *j && (i != e); ++i, ++j )
		*i						= *j;

	*i							= 0;
	return						0;
}

inline int __cdecl xr_sprintf	( char* destination, size_t const buffer_size, const char* format_string, ... )
{
	va_list args;
	va_start					( args, format_string);
	return						vsnprintf_s( destination, buffer_size, buffer_size - 1, format_string, args );
}

template <int count>
inline int __cdecl xr_sprintf	( char (&destination)[count], const char* format_string, ... )
{
	va_list args;
	va_start					( args, format_string);
	return						vsnprintf_s( destination, count, count - 1, format_string, args );
}
#endif // #ifndef MASTER_GOLD

//#	pragma deprecated( strcpy, strcpy_s, sprintf, sprintf_s, strcat, strcat_s )

template <int count>
inline errno_t xr_strcpy	( char (&destination)[count], const char* source )
{
	return						xr_strcpy( destination, count, source );
}

template <int count>
inline errno_t xr_strcat	( char (&destination)[count], const char* source )
{
	return						xr_strcat( destination, count, source );
}
#endif // #ifndef _EDITOR

XRCORE_API	char*				timestamp				(string64& dest);

extern XRCORE_API u32			crc32					(const void* P, u32 len);
extern XRCORE_API u32			crc32					(const void* P, u32 len, u32 starting_crc);
extern XRCORE_API u32			path_crc32				(const char* path, u32 len); // ignores '/' and '\'
