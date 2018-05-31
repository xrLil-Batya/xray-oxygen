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