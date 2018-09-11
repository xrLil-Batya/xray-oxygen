#include "common.h"
#include "mblur.h"
#include "dof.h"

#ifndef USE_MSAA
Texture2D s_distort;
#else
Texture2DMS<float4, MSAA_SAMPLES> s_distort;
#endif

struct c2_out
{
	float4	Color : SV_Target;
#ifdef USE_MSAA
	float	Depth : SV_Depth;
#endif
};

c2_out main(p_screen I)
{
	c2_out res;

	int		iSample = 0;
	float2	center	= I.tc0;

#ifdef GBUFFER_OPTIMIZATION
	gbuffer_data gbd = gbuffer_load_data(I.tc0, I.hpos, iSample);
#else
	gbuffer_data gbd = gbuffer_load_data(I.tc0, iSample);
#endif
	
#ifdef USE_DISTORT
	float4 	distort	= s_distort.Load(int3(I.hpos.xy, 0), iSample);
	float2	offset	= (distort.xy - (127.0f/255.0f))*def_distort;  // fix newtral offset
	
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
	float2	tc_offs	= I.tc0.xy + offset;
	
	float	m_lt	= s_distort.Load(int3((tc_offs + float2(-3.0f,  3.0f) * screen_res.zw) * screen_res.xy, 0), iSample).w;	m_lt = step(0.00001f, m_lt);
	float	m_t		= s_distort.Load(int3((tc_offs + float2( 0.0f,  3.0f) * screen_res.zw) * screen_res.xy, 0), iSample).w;	m_t	 = step(0.00001f, m_t);
	float	m_rt	= s_distort.Load(int3((tc_offs + float2( 3.0f,  3.0f) * screen_res.zw) * screen_res.xy, 0), iSample).w;	m_rt = step(0.00001f, m_rt);
	float	m_l		= s_distort.Load(int3((tc_offs + float2(-3.0f,  0.0f) * screen_res.zw) * screen_res.xy, 0), iSample).w;	m_l  = step(0.00001f, m_l);
	float	m_c		= s_distort.Load(int3((tc_offs) * screen_res.xy, 0), iSample).w;										m_c  = step(0.00001f, m_c);
	float	m_r		= s_distort.Load(int3((tc_offs + float2( 3.0f,  0.0f) * screen_res.zw) * screen_res.xy, 0), iSample).w;	m_r  = step(0.00001f, m_r);
	float	m_lb	= s_distort.Load(int3((tc_offs + float2(-3.0f, -3.0f) * screen_res.zw) * screen_res.xy, 0), iSample).w;	m_lb = step(0.00001f, m_lb);
	float	m_b		= s_distort.Load(int3((tc_offs + float2( 0.0f, -3.0f) * screen_res.zw) * screen_res.xy, 0), iSample).w;	m_b  = step(0.00001f, m_b);
	float	m_rb	= s_distort.Load(int3((tc_offs + float2( 3.0f, -3.0f) * screen_res.zw) * screen_res.xy, 0), iSample).w;	m_rb = step(0.00001f, m_rb);
	// Average mask and clip transitions to black.
	float	mask	= dot(m_lt + m_t + m_rt + m_l + m_c + m_r + m_lb + m_b + m_rb, 1.0f / 9.0f);
			mask	= step(0.99f, mask);
	
	// Apply distortion offset dependent on mask.
	center += offset * mask;
#endif // USE_DISTORT
	float3	img		= dof(center);
	float4 	bloom	= s_bloom.Sample(smp_rtlinear, center);
			img 	= mblur(center, gbd.P, img.rgb);
#ifdef USE_DISTORT
 	float3	blurred	= bloom*def_hdr	;
			img		= lerp(img, blurred, distort.z);
#endif
	res.Color		= combine_bloom(img, bloom);
#ifdef USE_MSAA
	float4	ptp		= mul(m_P, float4(gbd.P, 1.0f));
	res.Depth		= (ptp.w == 0.0f) ? 1.0f : ptp.z/ptp.w;
#endif
	return res;
}
