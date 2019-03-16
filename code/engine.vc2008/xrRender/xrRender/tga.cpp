// file: targasaver.cpp
#include "stdafx.h"
#pragma hdrstop

#include "tga.h"

void BMPdesc::makebmp(IWriter& fs)
{
	R_ASSERT(data);
	R_ASSERT(width);
	R_ASSERT(height);

	BMPHeader hdr;
    memset(&hdr, 0, sizeof(hdr));
	hdr.bmpImgType			= 2;
	hdr.bmpImgSpec.bmpXSize = u16(width);
	hdr.bmpImgSpec.bmpYSize = u16(height);

	switch(format)
	{
	case IMG_16B:
		hdr.bmpImgSpec.bmpDepth = 16;
		hdr.bmpImgSpec.bmpImgDesc = 0x10;
		break;
	case IMG_24B:
		hdr.bmpImgSpec.bmpDepth = 24;
		hdr.bmpImgSpec.bmpImgDesc = 0x20; // flip
		break;
	case IMG_32B:
		hdr.bmpImgSpec.bmpDepth = 32;
		hdr.bmpImgSpec.bmpImgDesc = 0xF | 0x20;
		break;
	}

	fs.w(&hdr, sizeof(hdr));

	if(format == IMG_24B)
	{
		BYTE ab_buffer[4]	= {0,0,0,0};
		int  real_sl		= ((width*3)) & 3;
		int  ab_size		= real_sl ? 4-real_sl : 0 ;
		for (int j = 0; j < height; j++)
		{
			BYTE *p = (LPBYTE)data + scanlenght * j;
			for (int i = 0; i < width; i++) 
			{
				BYTE buffer[3] = { p[0],p[1],p[2] };
				fs.w(buffer, 3);
				p += 4;
			}
			if (ab_size)
				fs.w(ab_buffer, ab_size);
		}
	}
	else if (format == IMG_32B)
	{
		if (width * 4 == scanlenght)
			fs.w(data, width*height * 4);
		else
		{
			// bad pitch, it seems :(
			for (int j = 0; j < height; j++)
			{
				BYTE *p = (LPBYTE)data + scanlenght * j;
				for (int i = 0; i < width; i++)
				{
					BYTE buffer[4] = { p[0],p[1],p[2],p[3] };
					fs.w(buffer, 4);
					p += 4;
				}
			}
		}
	}
	else // IMG_16B
	{
		BYTE ab_buffer[2]	= { 0,0 };
		int  real_sl		= ((width * 3)) & 3;
		int  ab_size		= real_sl ? 4 - real_sl : 0;

		if (width * 4 == scanlenght)
			fs.w(data, width*height * 4);
		else
		{
			for (int j = 0; j < height; j++)
			{
				BYTE *p = (LPBYTE)data + scanlenght * j;
				for (int i = 0; i < width; i++)
				{
					BYTE buffer[2] = { p[0],p[1] };
					fs.w(buffer, 2);
					p += 4;
				}
			}
			if (ab_size)
				fs.w(ab_buffer, ab_size);
		}

	}
}