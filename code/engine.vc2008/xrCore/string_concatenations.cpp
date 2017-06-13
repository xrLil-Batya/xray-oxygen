#include "stdafx.h"
#include "string_concatenations.h"

namespace xray {
	
namespace core {
	
namespace detail {

namespace strconcat_error 
{
	void process(u32 const index, u32 const count, const char* *strings)
	{
		u32 constexpr	max_string_size = 1024;
		char* temp = (char*)_alloca((count*(max_string_size + 4) + 1) * sizeof(**strings));
		char* k = temp;
		*k++ = '[';
		for (u32 i = 0; i < count; ++i) {
			for (const char* j = strings[i], *e = j + max_string_size; *j && j < e; ++k, ++j)
				*k = *j;

			*k++ = ']';

			if (i + 1 >= count)
				continue;

			*k++ = '[';
			*k++ = '\r';
			*k++ = '\n';
		}
		*k = 0;

		Debug.fatal(DEBUG_INFO, make_string("buffer overflow: cannot concatenate strings(%d):\r\n%s", index, temp).c_str());
	}

template <u32 count>
static inline void process	(char*& i, const char* e, u32 const index, const char* (&strings)[count])
{
	VERIFY		(i <= e);
	VERIFY		(index < count);

	if (i != e)
		return;

#ifndef MASTER_GOLD
	process		(index, count, strings);
#else // #ifndef MASTER_GOLD
	--i;
#endif // #ifndef MASTER_GOLD
}

} // namespace strconcat_error 

int stack_overflow_exception_filter	(int _exception_code)
{
	// Do not call _resetstkoflw here, because
	// at this point, the stack is not yet unwound.
	// Instead, signal that the handler (the __except block)
	// is to be executed.
	return (_exception_code == EXCEPTION_STACK_OVERFLOW) ? 1 : 0;
}

void   check_stack_overflow(u32 stack_increment)
{
	__try
	{
		void* p = _alloca(stack_increment);
		p;
	}
	__except (xray::core::detail::stack_overflow_exception_filter(GetExceptionCode()))
	{
		_resetstkoflw();
	}
}

void	string_tupples::error_process() const
{
	const char* strings[6];

	u32 part_size = 0;
	u32 overrun_string_index = (u32)-1;
	for (u32 i = 0; i < m_count; ++i)
	{
		strings[i] = m_strings[i].first;

		if (overrun_string_index == (u32)-1)
		{
			part_size += m_strings[i].second;
			if (part_size > max_concat_result_size)
				overrun_string_index = i;
		}
	}
	VERIFY(overrun_string_index != -1);

	strconcat_error::process(overrun_string_index, m_count, strings);
}

} // namespace detail

} // namespace core
 
} // namespace xray

using namespace xray::core::detail;
// dest = S1+S2+S3+S4+S5+S6
char* strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4, const char* S5, const char* S6)
{
	VERIFY(dest);
	VERIFY(S1);
	VERIFY(S2);

	const char* strings[]		= { S1, S2, S3, S4, S5, S6 };
	const char* strings_old[]	= { S1, S2, S3, S4, S5, S6 };

	char*				i = dest;
	const char*			e = dest + dest_sz;
	const char*			j;
	for (u32 step = 0; step < 6; step++)
	{
		for (j = strings_old[step]; *j && i < e; ++i, ++j)
			*i = *j;

		strconcat_error::process(i, e, step, strings);
	}
	/*
	for (j = S2; *j && i < e; ++i, ++j)
		*i = *j;
	strconcat_error::process(i, e, 1, strings);

	for (j = S3; *j && i < e; ++i, ++j)
		*i = *j;
	strconcat_error::process(i, e, 2, strings);

	for (j = S4; *j && i < e; ++i, ++j)
		*i = *j;
	strconcat_error::process(i, e, 3, strings);

	for (j = S5; *j && i < e; ++i, ++j)
		*i = *j;
	strconcat_error::process(i, e, 4, strings);

	for (j = S6; *j && i < e; ++i, ++j)
		*i = *j;
	strconcat_error::process(i, e, 5, strings);
	*/
	*i = 0;
	return (dest);
}
