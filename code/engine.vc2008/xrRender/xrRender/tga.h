// file: targasaver.h
#pragma once

#pragma pack(push,1)
struct BMPImgSpecHeader
{
	u16		bmpXOrigin;
	u16		bmpYOrigin;
	u16		bmpXSize;
	u16		bmpYSize;
	BYTE	bmpDepth;
	BYTE	bmpImgDesc;
};
struct BMPHeader
{
	BYTE	bmpIDL;
	BYTE	bmpMapType;
	BYTE	bmpImgType;
	BYTE	bmpClrMapSpec[5];
	BMPImgSpecHeader bmpImgSpec;
};
#pragma pack(pop)

#define IMG_16B 0
#define IMG_24B 1
#define IMG_32B 2

class BMPdesc
{
public:
	int format;
	int scanlenght;
	int width,height;
	void *data;
public:
	BMPdesc()	{ data = 0; };
	~BMPdesc()	{};

	void makebmp( IWriter &fs );
};