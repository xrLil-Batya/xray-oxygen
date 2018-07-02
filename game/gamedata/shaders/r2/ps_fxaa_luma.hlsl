#include "common.h"

struct v2p 
{
	half4	HPos : POSITION;
	half2	tc0	 : TEXCOORD0;
};

half4 main(v2p I) : COLOR
{
    half4 	img 	= tex2D(s_image, I.tc0);
			img.a	= dot(img.rgb, LUMINANCE_VECTOR);
			
    return img;
}