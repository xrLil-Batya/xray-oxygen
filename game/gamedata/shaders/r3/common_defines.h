#ifndef	common_defines_h_included
#define	common_defines_h_included

//////////////////////////////////////////////////////////////////////////////////////////
// Defines
#define def_gloss       	float(2.0f / 255.0f)
#define def_aref        	float(200.0f / 255.0f)
#define def_dbumph      	float(0.333f)
#define def_virtualh    	float(0.05f)	// 5cm
#define def_distort     	float(0.05f)	// we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
#define def_hdr         	float(9.0f)		// high luminance range float(3.0f)
#define def_hdr_clip		float(0.75f)
#define ALPHATEST(x)		clip((x) - (1.0f - 0.75f));
#define	LUMINANCE_VECTOR	float3(0.3f, 0.38f, 0.22f)

//////////////////////////////////////////////////////////////////////////////////////////
#ifndef SMAP_size
#define SMAP_size 1024
#endif
//////////////////////////////////////////////////////////////////////////////////////////

#define SKY_DEPTH	float(10000.f)
#define SKY_EPS		float(0.001)

#endif	//	common_defines_h_included
