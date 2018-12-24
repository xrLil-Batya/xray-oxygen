#include "common.h"
#include "lmodel.h"
#include "shadow.h"

#ifdef MSAA_OPTIMIZATION
#define MSAA_OPT_ARGS , uint iSample : SV_SAMPLEINDEX
#else
#define MSAA_OPT_ARGS
#endif

float4 main(p_aa_AA_sun I, float4 pos2d : SV_Position MSAA_OPT_ARGS) : SV_Target
{
	gbuffer_data gbd = gbuffer_load_data( GLD_P(I.tc, pos2d, ISAMPLE) );
	
	float4 	_P	= float4( gbd.P, gbd.mtl );
	float4	_N	= float4( gbd.N, gbd.hemi );

	// ----- light-model
	float 	m	= xmaterial	;
# ifndef USE_R2_STATIC_SUN
			m 	= _P.w		;
# endif
	float4	light	= plight_infinity( m, _P, _N, Ldynamic_dir );

	// ----- shadow
	float4 	s_sum;
		s_sum.x	= s_smap.Sample( smp_nofilter, I.LT).x;
		s_sum.y = s_smap.Sample( smp_nofilter, I.RT).y;
		s_sum.z	= s_smap.Sample( smp_nofilter, I.LB).z;
		s_sum.w = s_smap.Sample( smp_nofilter, I.RB).w;
		
	float 	s 	= (s_sum.x+s_sum.y + s_sum.z+s_sum.w) * 0.25f;

	return Ldynamic_color * light * s;
}
