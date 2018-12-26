#include "common.h"

//GSC is silly :P They used s_image instead of s_bloom, which is why this never worked.
//The normal filtering is very inefficient. Try using this instead ("r_ls_bloom_fast on")
//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
float4 main( p_build I ) : SV_Target
{

	float4	t_0	= s_bloom.Sample( smp_rtlinear, I.Tex0);
	float4	t_1	= s_bloom.Sample( smp_rtlinear, I.Tex1);
	float4	t_2	= s_bloom.Sample( smp_rtlinear, I.Tex2);
	float4	t_3	= s_bloom.Sample( smp_rtlinear, I.Tex3);

        // out
	return  ( (t_0+t_1) + (t_2+t_3) ) / 2;
}
