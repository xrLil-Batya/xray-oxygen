#include "stdafx.h"
#pragma hdrstop

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
	if (CPU::Info.hasFeature(CPUFeature::XFSR))
	{
		//#VERTVER to ForserX: € сделаль
		_fxsave64		(xrMemFill64_SSE2);
		_fxrstor64		(xrMemFill64_SSE2);
	}
}

//void	__stdcall	xrMemFill32_MMX(LPVOID dest, u32 value, u32 count)
//{
//
//	__asm 
//	{
//		MOV EDI, [dest];								// pointer to dst,DWORD aligned
//		MOV ECX, [count];							// number of DWORDs to copy
//		MOVD MM0, [value];							// initialization data
//		PUNPCKLDQ MM0, MM0;							// extend fill data to QWORD
//		CMP ECX, 1;									// less than one DWORD to fill ?
//		JB $filldone2_fc;							// yes,must be no DWORDs to fill,done
//		TEST EDI, 7;									// dst QWORD aligned?
//		JZ $dstqaligned2_fc;						// yes
//		MOVD[EDI], MM0;							// store one DWORD to dst
//		ADD EDI, 4;									// dst++
//		DEC ECX;									// number of DWORDs to fill
//
//	$dstqaligned2_fc:
//
//		MOV EBX, ECX;								// number of DWORDs to fill
//		SHR ECX, 4;									// number of cache lines to fill
//		JZ $fillqwords2_fc;							// no whole cache lines to fill,maybe QWORDs
//		ALIGN 16;									// align loop for optimal performance
//
//	$cloop2_fc:
//
//		ADD EDI, 64;									// dst++
//		MOVQ[EDI - 64], MM0;							// store 1st DWORD in cache line to dst
//		MOVQ[EDI - 56], MM0;							// store 2nd DWORD in cache line to dst
//		MOVQ[EDI - 48], MM0;							// store 3rd DWORD in cache line to dst
//		MOVQ[EDI - 40], MM0;							// store 4th DWORD in cache line to dst
//		MOVQ[EDI - 32], MM0;							// store 5th DWORD in cache line to dst
//		MOVQ[EDI - 24], MM0;							// store 6th DWORD in cache line to dst
//		MOVQ[EDI - 16], MM0;							// store 7th DWORD in cache line to dst
//		DEC ECX;									// count--
//		MOVQ[EDI - 8], MM0;							// store 8th DWORD in cache line to dst
//		JNZ $cloop2_fc;								// until no more cache lines to copy
//
//	$fillqwords2_fc:
//
//		MOV ECX, EBX;								// number of DWORDs to fill
//		AND EBX, 0xE;								// number of QWORDS left to fill *2
//		JZ $filldword2_fc;							// no QWORDs left,maybe DWORD left
//		ALIGN 16;									// align loop for optimal performance
//
//	$qloop2_fc:
//
//		MOVQ[EDI], MM0;							// store QWORD to dst
//		ADD EDI, 8;									// dst++
//		SUB EBX, 2;									// count--
//		JNZ $qloop2_fc;								// until no more QWORDs left to copy
//
//	$filldword2_fc:
//
//		TEST ECX, 1;									// DWORD left to fill ?
//		JZ $filldone2_fc;							// nope,we Тre done
//		MOVD[EDI], MM0;							// store last DWORD to dst
//
//	$filldone2_fc:
//
//		EMMS;										// clear MMX state
//	}
//}
