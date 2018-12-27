#include "common.h"
#include "mblur.h"
#include "dof.h"


#ifdef USE_SSR //warning: unoptimized/wip. Don't turn on without a beefy computer!
//Shader from "Wicked Engine"

// Avoid stepping zero distance
static const float	g_fMinRayStep = 0.01f;
// Crude raystep count
static const int	g_iMaxSteps = 8;
// Crude raystep scaling
static const float	g_fRayStep = 1.18f;
// Fine raystep count
static const int	g_iNumBinarySearchSteps = 8;
// Approximate the precision of the search (smaller is more precise)
static const float  g_fRayhitThreshold = 0.9f;
#define g_xFrame_MainCamera_ZFarP 150.0
bool bInsideScreen(in float2 vCoord)
{
	if (vCoord.x < 0 || vCoord.x > 1 || vCoord.y < 0 || vCoord.y > 1)
		return false;
	return true;
}

float4 SSRBinarySearch(float3 vDir, inout float3 vHitCoord)
{
	float fDepth;

	for (int i = 0; i < g_iNumBinarySearchSteps; i++)
	{
		float4 vProjectedCoord = mul(float4(vHitCoord, 1.0f), m_P);
		vProjectedCoord.xy /= vProjectedCoord.w;
		vProjectedCoord.xy = vProjectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

		fDepth = s_position.SampleLevel(smp_nofilter, vProjectedCoord.xy, 0).z * g_xFrame_MainCamera_ZFarP;
		float fDepthDiff = vHitCoord.z - fDepth;

		if (fDepthDiff <= 0.0f)
			vHitCoord += vDir;

		vDir *= 0.5f;
		vHitCoord -= vDir;
	}

	float4 vProjectedCoord = mul(float4(vHitCoord, 1.0f), m_P);
	vProjectedCoord.xy /= vProjectedCoord.w;
	vProjectedCoord.xy = vProjectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

	fDepth = s_position.SampleLevel(smp_nofilter, vProjectedCoord.xy, 0).z * g_xFrame_MainCamera_ZFarP;
	float fDepthDiff = vHitCoord.z - fDepth;

	return float4(vProjectedCoord.xy, fDepth, abs(fDepthDiff) < g_fRayhitThreshold ? 1.0f : 0.0f);
}

float4 SSRRayMarch(float3 vDir, inout float3 vHitCoord)
{
	float fDepth;

	for (int i = 0; i < g_iMaxSteps; i++)
	{
		vHitCoord += vDir;

		float4 vProjectedCoord = mul(float4(vHitCoord, 1.0f), m_P);
		vProjectedCoord.xy /= vProjectedCoord.w;
		vProjectedCoord.xy = vProjectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

		fDepth = s_position.SampleLevel(smp_nofilter, vProjectedCoord.xy, 0).z * g_xFrame_MainCamera_ZFarP;

		float fDepthDiff = vHitCoord.z - fDepth;

		[branch]
		if (fDepthDiff > 0.0f)
			return SSRBinarySearch(vDir, vHitCoord);

		vDir *= g_fRayStep;

	}

	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

void SSR(float3 N, float3 P)
{
	//float3 N = decode(texture_gbuffer1.Load(int3(input.pos.xy, 0)).xy);
	//float3 P = getPosition(input.tex, texture_depth.Load(int3(input.pos.xy, 0)));


	//Reflection vector
	float3 vViewPos = mul(float4(P.xyz, 1), m_P).xyz;
	float3 vViewNor = mul(float4(N, 0), m_P).xyz;
	float3 vReflectDir = normalize(reflect(vViewPos.xyz, vViewNor.xyz));


	//Raycast
	float3 vHitPos = vViewPos;

	float4 vCoords = SSRRayMarch(vReflectDir /** max( g_fMinRayStep, vViewPos.z )*/, vHitPos);

	float2 vCoordsEdgeFact = float2(1, 1) - pow(saturate(abs(vCoords.xy - float2(0.5f, 0.5f)) * 2), 8);
	float fScreenEdgeFactor = saturate(min(vCoordsEdgeFact.x, vCoordsEdgeFact.y));


	//Color
	float reflectionIntensity =
		saturate(
			fScreenEdgeFactor *		// screen fade
			saturate(vReflectDir.z)	// camera facing fade
			* vCoords.w				// rayhit binary fade
			);


	float3 reflectionColor = s_image.SampleLevel(smp_rtlinear, vCoords.xy, 0).rgb;
reflectionColor *= reflectionIntensity;
	//return max(0, float4(reflectionColor, reflectionIntensity));

}
#endif



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

//[earlydepthstencil]
c2_out main(p_screen I)
{
	c2_out res;

	int		iSample = 0;
	float2	center	= I.tc0;

	gbuffer_data gbd = gbuffer_load_data(I.tc0, I.hpos, iSample);
	
#ifdef USE_DISTORT //This looks fantastic, but it seems more expensive than GSC's method. Maybe 4x4 pixels instead of 7x7?
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
	float4	ptp		= mul(m_P, float4(gbd.P, 1.0f));
#ifdef USE_SSR	
    SSR(gbd.N, ptp.xyz);	
#endif		

#ifdef USE_DISTORT
 	float3	blurred	= bloom*def_hdr	;
			img		= lerp(img, blurred, distort.z);
#endif
	res.Color		= float4(img + bloom * bloom.a, 1.0);
#ifdef USE_MSAA
	res.Depth		= (ptp.w == 0.0f) ? 1.0f : ptp.z/ptp.w;
#endif
	return res;
}
