#ifndef BLUR_BOKEH_H_INCLUDED
#define BLUR_BOKEH_H_INCLUDED
/* 
DoF with bokeh by Martins Upitis (martinsh) (devlog-martinsh.blogspot.com) 
https://github.com/orthecreedence/ghostie/blob/master/opengl/glsl/dof.bokeh.2.4.frag 

------------------------------------------
Перенос шейдера глубины резкости с боке из OGSE 
Большое спасибо за помощь SGM 3.0 Team 
Перенес на XrOxygen - NuclearFG
*/

#include "shared\common.h"
#include "ogse_config.h"

#if !defined(BOKEH_QUALITY)
	#define	BOKEH_RINGS int(1)
	#define	BOKEH_SAMPLES int(1)
	#define BOKEH_QUALITY 1
#elif BOKEH_QUALITY==1
    #define BOKEH_RINGS int(3)
    #define BOKEH_SAMPLES int(3)
#elif BOKEH_QUALITY==2
    #define BOKEH_RINGS int(5)
    #define BOKEH_SAMPLES int(4)
#elif BOKEH_QUALITY==3
    #define BOKEH_RINGS int(7)
    #define BOKEH_SAMPLES int(4)
#elif BOKEH_QUALITY==4
    #define BOKEH_RINGS int(9)
    #define BOKEH_SAMPLES int(4)
#else
	#define	BOKEH_RINGS int(1)
	#define	BOKEH_SAMPLES int(1)
#endif


static const float2 inv_resolution = screen_res.zw*2;
static const float2 resolution = screen_res.xy*0.5f;

// pentagonal shape
float penta(float2 coords)
{
	float scale = BOKEH_RINGS - 1.3f;
	float4  HS0 = float4( 1.0,         0.0,         0.0,  1.0);
	float4  HS1 = float4( 0.309016994, 0.951056516, 0.0,  1.0);
	float4  HS2 = float4(-0.809016994, 0.587785252, 0.0,  1.0);
	float4  HS3 = float4(-0.809016994,-0.587785252, 0.0,  1.0);
	float4  HS4 = float4( 0.309016994,-0.951056516, 0.0,  1.0);
	float4  HS5 = float4( 0.0        ,0.0         , 1.0,  1.0);
	
	float4  one = float4( 1.0, 1.0, 1.0, 1.0 );
	
	float4 P = float4(coords.xy, scale, scale); 
	
	float4 dist = float4(0.0, 0.0, 0.0, 0.0);
	float inorout = -4.0;
	
	dist.x = dot( P, HS0 );
	dist.y = dot( P, HS1 );
	dist.z = dot( P, HS2 );
	dist.w = dot( P, HS3 );
	
	dist = smoothstep( -BOKEH_FEATHER, BOKEH_FEATHER, dist );
	
	inorout += dot( dist, one );
	
	dist.x = dot( P, HS4 );
	dist.y = HS5.w - abs( P.z );
	
	dist = smoothstep( -BOKEH_FEATHER, BOKEH_FEATHER, dist );
	inorout += dist.x;
	
	return saturate( inorout );
}

float3 color(float2 coords,float blur, float2 color_scale) {	//processing the sample
	float3 col;

	col.r = tex2Dlod(s_image,float4(coords.xy,0.0f, 0.0f) + float4( 0.0f,	 1.0f, 0.0f, 0.0f)*float4(color_scale.xy, 0.0f, 0.0f)).r;
	col.g = tex2Dlod(s_image,float4(coords.xy,0.0f, 0.0f) + float4(-0.866f, -0.5f, 0.0f, 0.0f)*float4(color_scale.xy, 0.0f, 0.0f)).g;
	col.b = tex2Dlod(s_image,float4(coords.xy,0.0f, 0.0f) + float4( 0.866,  -0.5,  0.0f, 0.0f)*float4(color_scale.xy, 0.0f, 0.0f)).b;

	float lum = dot(col.rgb, LUMINANCE_VECTOR);
	float thresh = max((lum-BOKEH_THRESHOLD)*BOKEH_GAIN, 0.0f);
	return (col + lerp(float3(0.0f, 0.0f, 0.0f), col, thresh*blur));
}
float2 rand(float2 coord)
{
	// generating noise/pattern texture for dithering
	float2 noise;
	noise.x = ((frac(1.0f - coord.x*(resolution.x*0.5f))*0.25f) + (frac(coord.y*(resolution.y*0.5f))*0.75f))*2.0f - 1.0f;
	noise.y = ((frac(1.0f - coord.x*(resolution.x*0.5f))*0.75f) + (frac(coord.y*(resolution.y*0.5f))*0.25f))*2.0f - 1.0f;
	return noise;
}

float3 bokeh_dof(float2 center, float blur)
{
	// calculation of pattern for ditering	
	float2 noise = rand(center)*0.0001f;
	
	// getting blur x and y step factor
	float2 texel = inv_resolution.xy;
	float2 scale = (texel*BOKEH_KERNEL+noise)*blur/BOKEH_QUALITY;	
	// calculation of final color
	float3 col = tex2D(s_image, center).rgb;
	float2 color_scale = texel*BOKEH_FRINGE*blur;
	if (blur >= 0.05f) // some optimization thingy
	{
		float s = 1.0f;
		int ringsamples = 0;
		for (int i = 1; i <= BOKEH_RINGS; i++)
		{
			ringsamples = i * BOKEH_SAMPLES;
			for (int j = 0; j < ringsamples; j++)
			{
				float2 pwh = float2(1.0f,1.0f);
				float stp = (PI*2.0f)*j / ringsamples;
				sincos(stp, pwh.y, pwh.x);
				pwh *= i;
				float p = penta(pwh);

				col += color(center + pwh*scale,blur, color_scale)*lerp(1.0f, i*(1.0f/BOKEH_RINGS), BOKEH_BIAS)*p;
				s += lerp(1.0f, i*(1.0f/BOKEH_RINGS), BOKEH_BIAS)*p; 
			}
		}
		col /= s; //divide by sample count
	}
	return col;
}
#endif //BLUR_BOKEH_H
