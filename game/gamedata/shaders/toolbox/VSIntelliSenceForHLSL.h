#pragma once
#define uniform
#define inout

using float4 = struct
{
	float x;
	float y;
	float z;
	float w;
};

using half4 = struct
{
	short x;
	short y;
	short z;
	short w;
};

using float3 = struct
{
	float x;
	float y;
	float z;
};

using half3 = struct
{
	short x;
	short y;
	short z;
};

using float2 = struct
{
	float x;
	float y;
};

using half2 = struct
{
	short x;
	short y;
};