#include "common.h"
//Fixed glows by skyloader
struct	p_TL
{
	float2 	Tex0	: TEXCOORD0;
	float4	Color	: COLOR;
	float4  tctexgen: TEXCOORD1;
};

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
//////////////////////////////////////////////////////////////////////////////////////////

//	Must be less than view near
#define	DEPTH_EPSILON	0.1f

float4 main( p_TL I ) : COLOR
{
	float4	t_base 	= ( tex2D( s_base, I.Tex0 ) * I.Color ); 		//aggresive glows
//	float4	t_base 	= ( tex2D( s_base, I.Tex0 ) * I.Color ) / 1.5f; 	//soft glows

	float4	_P	= tex2Dproj(s_position, I.tctexgen);

	float spaceDepth = _P.z-I.tctexgen.z-DEPTH_EPSILON;
	if (spaceDepth < -2*DEPTH_EPSILON ) spaceDepth = 100000.0f; //  filter for skybox

	t_base.a *= Contrast( saturate(spaceDepth*1.3f), 2);
	t_base.rgb *= Contrast( saturate(spaceDepth*1.3f), 2);

	return  t_base;
}
