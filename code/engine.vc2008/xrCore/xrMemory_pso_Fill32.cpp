#include "stdafx.h"
#pragma hdrstop
#include <intrin.h>
void	__stdcall	xrMemFill32_x86(LPVOID dest, u32 value, u32 count)
{
	u32 *ptr = (u32*)dest;
	u32 *end = ptr + count;
	for (; ptr != end; )
		*ptr++ = value;
}

void	__stdcall	xrMemFill32_MMX(LPVOID dest, u32 value, u32 count)
{
	u32 *ptr = (u32*)dest;
	u32 *end = ptr + count;
	for (; ptr != end; )
		*ptr++ = value;
}

// Don't fucking use this now, that's didn't working
void xrMemFill64_SSE2(LPVOID dest, u32 value, u32 count)
{
	__m128i mValue		= _mm_cvtsi32_si128(value);			// copy to lower elements
	__m128i mCount		= _mm_cvtsi32_si128(count);			
	__m128i mDest		= _mm_cvtsi32_si128((u32)dest);
	__m128i mPunp		= _mm_unpacklo_epi32(mValue, mValue);	// unpack the
#ifdef CRAZY_INSTRUCTIONS		// why not
	__m128 mmValue = _mm_cvti32_ss(mmValue, value);		//AVX-512F
	//__m512 SuperValue = _mm512_zextps128_ps512(mmValue);
	//__m512i SuperPunp = _mm512_unpacklo_epi32();
#endif
}
