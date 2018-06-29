#ifndef EXTCOMMON_H_INCLUDED
#define EXTCOMMON_H_INCLUDED

//////////////////////////////////////////////////////////////////////////////////////////
//
struct		AntiAliasingStruct
{
	float2		texCoord0 : TEXCOORD0;
	float2		texCoord1 : TEXCOORD1;
	float2		texCoord2 : TEXCOORD2;
	float2		texCoord3 : TEXCOORD3;
	float2		texCoord4 : TEXCOORD4;
	float4		texCoord5 : TEXCOORD5;
	float4		texCoord6 : TEXCOORD6;
};

// Rain drops
uniform	Texture2D sScene;
uniform	Texture2D sPosition;
uniform	Texture2D sMask;          //sunshafts occlusion mask
uniform	Texture2D sMaskBlur;      //smoothed mask
uniform	Texture2D sSunShafts;     //

uniform float4 SSParams;
uniform float4 SSParamsDISPLAY;

Texture2D s_rain_drops0;
uniform float2 rain_drops_params0;
//#define RAIN_DROPS_DEBUG

struct OptimizeAA
{
	float2 texCoord0 : TEXCOORD0;
};

float NormalizeDepth(float Depth)
{
	return saturate(Depth / 100.f);
}

float4 proj2screen(float4 Project)
{
	float4 Screen;
	Screen.x = (Project.x+Project.w)*0.5;
	Screen.y = (Project.w-Project.y)*0.5;
	Screen.z = Project.z;
	Screen.w = Project.w;
	return Screen;
}

#endif//EXTCOMMON_H_INCLUDED