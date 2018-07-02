#include "common.h"

float4 main(p_screen I) : SV_Target
{
    float4 	img 	= s_image.Sample(smp_rtlinear, I.tc0);
			img.a	= dot(img.rgb, LUMINANCE_VECTOR);
			
    return img;
}