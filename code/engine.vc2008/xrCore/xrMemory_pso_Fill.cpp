#include "stdafx.h"
#include <intrin.h>
#pragma hdrstop

void __stdcall xrMemFill_x86(void* dest, int value, u32 count)
{
	memset(dest, int(value), count);
}

#ifdef CRAZY_INSTRUCTIONS
// Don't fucking use this now, that's didn't working
void xrMemFill64_SSE2(LPVOID dest, u32 value, u32 count)
{
	__m128i mValue = _mm_cvtsi32_si128(value);			// copy to lower elements
	__m128i mCount = _mm_cvtsi32_si128(count);
	__m128i mDest = _mm_cvtsi32_si128((u32)dest);
	__m128i mPunp = _mm_unpacklo_epi32(mValue, mValue);	// unpack the
}
#endif

void __stdcall xrMemFill32_x86(LPVOID dest, u32 value, u32 count)
{
#ifdef CRAZY_INSTRUCTIONS
	if (CPU::Info.hasFeature(CPUFeature::SSE2))
	{
		xrMemFill64_SSE2(dest, value, count);
	}
	else
#endif
	{
		u32 *ptr = (u32*)dest;
		u32 *end = ptr + count;

		while (ptr != end)
		{
			*ptr++ = value;
		}
	}
}