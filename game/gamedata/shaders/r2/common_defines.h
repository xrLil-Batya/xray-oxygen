#ifndef COMMON_DEFINES_H
#define COMMON_DEFINES_H

// *** options

// #define DBG_TEST_NMAP
// #define DBG_TEST_NMAP_SPEC
// #define DBG_TEST_SPEC
// #define DBG_TEST_LIGHT
// #define DBG_TEST_LIGHT_SPEC

// #define USE_GAMMA_22
// #define USE_SJITTER
// #define USE_SUNFILTER
// #define USE_FETCH4
// #define USE_MBLUR                	//- HW-options defined
// #define USE_HWSMAP                	//- HW-options defined

// #define USE_HWSMAP_PCF				//- nVidia GF3+, R600+

// #define USE_BRANCHING        		//- HW-options defined
// #define USE_VTF                		//- HW-options defined, VertexTextureFetch
// #define FP16_FILTER                	//- HW-options defined
// #define FP16_BLEND                	//- HW-options defined

// #define USE_PARALLAX                	//- shader defined
// #define USE_TDETAIL                	//- shader defined
// #define USE_LM_HEMI                	//- shader defined
// #define USE_DISTORT                	//- shader defined
 #define USE_SUNMASK                	//- shader defined
// #define DBG_TMAPPING
// #define USE_SUPER_SPECULAR
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef SMAP_size
#define SMAP_size 1024
#endif

#ifdef USE_R2_STATIC_SUN
#  define xmaterial float(1.0f/4.0f)
#else
#  define xmaterial float(L_material.w)
#endif

#define SKY_DEPTH	float(10000.f)
#define SKY_EPS		float(0.001)

//////////////////////////////////////////////////////////////////////////////////////////
// Defines //
#define def_gloss       float(2.f /255.f)
#define def_aref        float(200.f/255.f)
#define def_dbumph      float(0.333f)
#define def_virtualh    float(0.05f)			// 5cm
#define def_distort     float(0.05f)			// we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
#define def_hdr         float(9.0f)         	// hight luminance range float(3.h)
#define def_hdr_clip	float(0.75f)

//////////////////////////////////////////////////////////////////////////////////////////
#define	LUMINANCE_VECTOR float3(0.3f, 0.38f, 0.22f)

#endif // COMMON_DEFINES_H
