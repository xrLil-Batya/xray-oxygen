#include "common.h"

struct v2p_TLxxx 
{ 
	float2 Tex0 : TEXCOORD0; 
	float4 HPos : SV_Position; // Clip-space position (for rasterization) 
}; 

////////////////////////////////////////////////////////////////////////////////////////// 
// Vertex 
v2p_TL main(v_TL I) 
{ 
	v2p_TL O; 
	
	O.HPos = I.P; 
	O.Tex0 = I.Tex0; 
	
	return O; 
}