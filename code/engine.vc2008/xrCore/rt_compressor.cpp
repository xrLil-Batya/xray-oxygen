#include "stdafx.h"
#pragma hdrstop
#include "../../3rd-party/lzo/lzo1.h"

#define HEAP_ALLOC(var,size) \
	lzo_align_t var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

__declspec(thread) HEAP_ALLOC(rtc_wrkmem, LZO1_MEM_COMPRESS);


void XRay::Compress::RT::RtcInitialize()
{
	VERIFY(lzo_init()==LZO_E_OK);
}

u32 XRay::Compress::RT::RtcSize(u32 in)
{
	VERIFY(in);
	return in + in/64 + 16 + 3;
}

size_t XRay::Compress::RT::RtcCompress(void *dst, u32 dst_len, const void* src, u32 src_len)
{
	u64	out_size = dst_len;
	int r = lzo1_compress((const lzo_byte *)src, (lzo_uint)src_len, (lzo_byte *)dst, (lzo_uintp)&out_size, rtc_wrkmem);
	VERIFY(r == LZO_E_OK);
	return	out_size;
}

size_t XRay::Compress::RT::RtcDecompress(void *dst, u32 dst_len, const void* src, u32 src_len)
{
	u64 out_size = dst_len;
	int r = lzo1_decompress((const lzo_byte *)src, (lzo_uint)src_len, (lzo_byte *)dst, (lzo_uintp)&out_size, rtc_wrkmem);
	VERIFY(r == LZO_E_OK);
	return	out_size;
}


