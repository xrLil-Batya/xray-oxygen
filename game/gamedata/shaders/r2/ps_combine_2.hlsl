#include "common.h"
#include "mblur.h"
#include "dof.h"

#ifdef USE_DISTORT
uniform sampler2D s_distort;
#endif

float4 main (float4 tc0: TEXCOORD0) : COLOR
{
	float4 	pos		= tex2D(s_position, tc0.xy);
	float2	center	= tc0;
	
#ifdef USE_DISTORT
	float4 	distort	= tex2D(s_distort, tc0.xy);
	float2	offset	= (distort.xy - (127.0f/255.0f))*def_distort;  // Fix neutral offset
	
	// Check every pixel by distortion mask to prevent "color bleeding" of front objects into distortion.
	// We also need to make black areas of mask slightly thicker to avoid artifacts on edges.
	// So get average distortion value from 7x7 pixels area and replace grey pixels with black.
	//  ___________
	// | lt  t  rt |
	// |           |
	// | l   c   r |
	// |           |
	// | lb  b  rb |
	//  -----------
	float	m_lt	= tex2D(s_distort, tc0.xy + offset + float2(-3.0f,  3.0f) * screen_res.zw).w;	m_lt = step(0.00001f, m_lt);
	float	m_t		= tex2D(s_distort, tc0.xy + offset + float2( 0.0f,  3.0f) * screen_res.zw).w;	m_t	 = step(0.00001f, m_t);
	float	m_rt	= tex2D(s_distort, tc0.xy + offset + float2( 3.0f,  3.0f) * screen_res.zw).w;	m_rt = step(0.00001f, m_rt);
	float	m_l		= tex2D(s_distort, tc0.xy + offset + float2(-3.0f,  0.0f) * screen_res.zw).w;	m_l  = step(0.00001f, m_l);
	float	m_c		= tex2D(s_distort, tc0.xy + offset).w;											m_c  = step(0.00001f, m_c);
	float	m_r		= tex2D(s_distort, tc0.xy + offset + float2( 3.0f,  0.0f) * screen_res.zw).w;	m_r  = step(0.00001f, m_r);
	float	m_lb	= tex2D(s_distort, tc0.xy + offset + float2(-3.0f, -3.0f) * screen_res.zw).w;	m_lb = step(0.00001f, m_lb);
	float	m_b		= tex2D(s_distort, tc0.xy + offset + float2( 0.0f, -3.0f) * screen_res.zw).w;	m_b  = step(0.00001f, m_b);
	float	m_rb	= tex2D(s_distort, tc0.xy + offset + float2( 3.0f, -3.0f) * screen_res.zw).w;	m_rb = step(0.00001f, m_rb);
	// Average mask and clip transitions to black.
	float	mask	= dot(m_lt + m_t + m_rt + m_l + m_c + m_r + m_lb + m_b + m_rb, 1.0f / 9.0f);
			mask	= step(0.99f, mask);
	
	// Apply distortion offset dependent on mask.
	center += offset * mask;
#endif
	float3	final	= dof(center);
			final 	= mblur(center, pos, final.rgb);
			
	float4 	bloom	= tex2D(s_bloom, center);

#ifdef USE_DISTORT
	float3	blurred	= bloom*def_hdr;
			final	= lerp(final, blurred, distort.z);
#endif
	return combine_bloom(final, bloom);
}
