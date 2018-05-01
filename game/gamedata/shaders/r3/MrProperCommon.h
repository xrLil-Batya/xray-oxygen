#ifndef EXTCOMMON_H_INCLUDED
#define EXTCOMMON_H_INCLUDED

//////////////////////////////////////////////////////////////////////////////////////////
//
struct		AntiAliasingStruct
{
	float4		texCoord0		:		TEXCOORD0;
	float4		texCoord1		:		TEXCOORD1;
	float4		texCoord2		:		TEXCOORD2;
	float4		texCoord3		:		TEXCOORD3;
	float4		texCoord4		:		TEXCOORD4;
	float4		texCoord5		:		TEXCOORD5;
	float4		texCoord6		:		TEXCOORD6;
};

//////////////////////////////////////////////////////////////////////////////////////////
//
uniform		Texture2D		s_rain_drops0;

//////////////////////////////////////////////////////////////////////////////////////////
//
uniform 	float2 		rain_drops_params0;

#endif//EXTCOMMON_H_INCLUDED