#pragma once
namespace XRay
{
	namespace Compress
	{
		namespace LZ
		{
			XRCORE_API unsigned WriteLZ(HANDLE hf, void* d, unsigned size);
			XRCORE_API unsigned ReadLZ(HANDLE hf, void* &d, unsigned size);
			XRCORE_API void		CompressLZ(u8** dest, unsigned* dest_sz, void* src, unsigned src_sz);
			XRCORE_API void		DecompressLZ(u8** dest, unsigned* dest_sz, void* src, unsigned src_sz);
		}
	}
}

