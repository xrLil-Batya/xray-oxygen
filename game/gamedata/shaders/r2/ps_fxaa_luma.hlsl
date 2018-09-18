#include "common.h"

float4 main(p_screen I) : COLOR
{
    float4 	img 	= tex2D(s_image, I.tc0);
			img.a	= dot(img.rgb, LUMINANCE_VECTOR);
			
    return img;
}