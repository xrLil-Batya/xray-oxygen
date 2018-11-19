#ifndef	common_samplers_h_included
#define	common_samplers_h_included

#ifdef SM_2_0
#define AUTO_SAMPLER sampler2D
#else
#define AUTO_SAMPLER Texture2D
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// Geometry phase / deferring               	//

sampler 	smp_nofilter;   //	Use D3DTADDRESS_CLAMP,	D3DTEXF_POINT,			D3DTEXF_NONE,	D3DTEXF_POINT 
sampler 	smp_rtlinear;	//	Use D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,			D3DTEXF_NONE,	D3DTEXF_LINEAR 
sampler 	smp_linear;		//	Use	D3DTADDRESS_WRAP,	D3DTEXF_LINEAR,			D3DTEXF_LINEAR,	D3DTEXF_LINEAR
sampler 	smp_base;		//	Use D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, 	D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC

AUTO_SAMPLER 	s_base;		//	smp_base
#ifdef USE_MSAA
AUTO_SAMPLERMS<float4, MSAA_SAMPLES>	s_generic;	//	smp_generic
#else
AUTO_SAMPLER   s_generic;
#endif
AUTO_SAMPLER 	s_bump;             	//
AUTO_SAMPLER 	s_bumpX;                //
AUTO_SAMPLER 	s_detail;               //
AUTO_SAMPLER 	s_detailBump;           //	
AUTO_SAMPLER 	s_detailBumpX;          //	Error for bump detail
//AUTO_SAMPLER 	s_bumpD;                //
AUTO_SAMPLER 	s_hemi;             	//

AUTO_SAMPLER 	s_mask;             	//

AUTO_SAMPLER 	s_dt_r;                	//
AUTO_SAMPLER 	s_dt_g;                	//
AUTO_SAMPLER 	s_dt_b;                	//
AUTO_SAMPLER 	s_dt_a;                	//

AUTO_SAMPLER 	s_dn_r;                	//
AUTO_SAMPLER 	s_dn_g;                	//
AUTO_SAMPLER 	s_dn_b;                	//
AUTO_SAMPLER 	s_dn_a;                	//

//////////////////////////////////////////////////////////////////////////////////////////
// Lighting/shadowing phase                     //

sampler 	smp_material;

//uniform sampler2D       s_depth;                //
#ifdef USE_MSAA
AUTO_SAMPLERMS<float4, MSAA_SAMPLES>	s_position;	//	smp_nofilter or Load
AUTO_SAMPLERMS<float4, MSAA_SAMPLES>	s_normal;	//	smp_nofilter or Load
AUTO_SAMPLERMS<float4, MSAA_SAMPLES>	s_depth;	
#else
AUTO_SAMPLER	s_position;	//	smp_nofilter or Load
AUTO_SAMPLER	s_normal;	//	smp_nofilter or Load
AUTO_SAMPLER s_depth;
#endif
AUTO_SAMPLER	s_lmap;		// 2D/???cube projector lightmap
Texture3D	s_material;	//	smp_material
//uniform sampler1D       s_attenuate;        	//


//////////////////////////////////////////////////////////////////////////////////////////
// Combine phase                                //
#ifdef USE_MSAA
AUTO_SAMPLERMS<float4, MSAA_SAMPLES>	s_diffuse;	// rgb.a = diffuse.gloss
AUTO_SAMPLERMS<float4, MSAA_SAMPLES>	s_accumulator;      	// rgb.a = diffuse.specular
#else
AUTO_SAMPLER	s_diffuse;	// rgb.a = diffuse.gloss
AUTO_SAMPLER	s_accumulator;      	// rgb.a = diffuse.specular
#endif
//uniform sampler2D       s_generic;              //
AUTO_SAMPLER	s_bloom;	//
AUTO_SAMPLER	s_image;	// used in various post-processing
AUTO_SAMPLER	s_tonemap;	// actually MidleGray / exp(Lw + eps)


#endif	//	#ifndef	common_samplers_h_included