#pragma once
namespace XRay
{
	namespace Compress
	{
		namespace RT
		{
			XRCORE_API void		RtcInitialize();
			XRCORE_API size_t	RtcCompress(void *dst, u32 dst_len, const void* src, u32 src_len);
			XRCORE_API u32		RtcSize(u32 in);
			XRCORE_API size_t	RtcDecompress(void *dst, u32 dst_len, const void* src, u32 src_len);
		}
	}
}