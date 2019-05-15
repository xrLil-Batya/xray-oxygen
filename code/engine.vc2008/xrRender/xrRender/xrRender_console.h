#pragma once

extern void xrRender_initconsole();
extern BOOL xrRender_test_hw	();

// Common

extern ECORE_API u32			ps_r_smapsize;		//=	2048;
extern ECORE_API xr_token		q_smapsize_token[];

extern ECORE_API u32			ps_r_sun_shafts;	//=	0;
extern ECORE_API xr_token		qsun_shafts_token[];

extern ECORE_API u32			ps_r_ssao;			//=	0;
extern ECORE_API xr_token		qssao_token[];

extern ECORE_API u32			ps_r_ssao_mode;
extern ECORE_API xr_token		qssao_mode_token[];

extern ECORE_API u32			ps_r_sun_quality;	//=	0;
extern ECORE_API xr_token		qsun_quality_token[];

extern ECORE_API u32			ps_r_bokeh_quality;
extern ECORE_API xr_token		qbokeh_quality_token[];

extern ECORE_API u32			ps_r_pp_aa_mode;
extern ECORE_API xr_token		pp_aa_mode_token[];

extern ECORE_API u32			ps_r_pp_aa_quality;
extern ECORE_API xr_token		pp_aa_quality_token[];

extern ECORE_API int			ps_r_pp_aa_use_taa;
extern ECORE_API int			ps_r_Supersample;
extern ECORE_API int			ps_r_LightSleepFrames;
extern ECORE_API int			ps_r_SkeletonUpdate;

extern ECORE_API float			ps_r_Detail_l_ambient;
extern ECORE_API float			ps_r_Detail_l_aniso;
extern ECORE_API float			ps_r_Detail_density;
extern ECORE_API int			ps_r_Detail_radius;
extern ECORE_API float			ps_r_Detail_height;

extern ECORE_API float			ps_r_Tree_w_rot;
extern ECORE_API float			ps_r_Tree_w_speed;
extern ECORE_API float			ps_r_Tree_w_amp;
extern ECORE_API float			ps_r_Tree_SBC;		// scale bias correct
extern ECORE_API Fvector		ps_r_Tree_Wave;

extern ECORE_API float			ps_r_WallmarkTTL;
extern ECORE_API float			ps_r_WallmarkSHIFT;
extern ECORE_API float			ps_r_WallmarkSHIFT_V;

extern ECORE_API float			ps_r_LOD;
extern ECORE_API float			ps_r_GLOD_ssa_start;
extern ECORE_API float			ps_r_GLOD_ssa_end;
extern ECORE_API float			ps_r_ssaLOD_A;
extern ECORE_API float			ps_r_ssaLOD_B;
extern ECORE_API float			ps_r_ssaDISCARD;
extern ECORE_API float			ps_r_ssaDONTSORT;
extern ECORE_API float			ps_r_ssaHZBvsTEX;

extern ECORE_API int			ps_r_tf_Anisotropic;
extern ECORE_API float			ps_r_tf_Mipbias;

extern ECORE_API float			ps_r_dtex_range;
extern ECORE_API float			ps_r_pps_u;
extern ECORE_API float			ps_r_pps_v;

extern ECORE_API float			ps_r_df_parallax_h;
extern ECORE_API float			ps_r_df_parallax_range;

extern ECORE_API float			ps_r_gmaterial;

extern ECORE_API float			ps_r_tonemap_middlegray;
extern ECORE_API float			ps_r_tonemap_adaptation;
extern ECORE_API float			ps_r_tonemap_low_lum;
extern ECORE_API float			ps_r_tonemap_amount;

extern ECORE_API float			ps_r_ls_bloom_kernel_scale;	// gauss
extern ECORE_API float			ps_r_ls_bloom_kernel_g;		// gauss
extern ECORE_API float			ps_r_ls_bloom_kernel_b;		// bilinear
extern ECORE_API float			ps_r_ls_bloom_threshold;
extern ECORE_API float			ps_r_ls_bloom_speed;
extern ECORE_API float			ps_r_ls_dsm_kernel;
extern ECORE_API float			ps_r_ls_psm_kernel;
extern ECORE_API float			ps_r_ls_ssm_kernel;
extern ECORE_API float			ps_r_ls_depth_scale;		// 1.0f
extern ECORE_API float			ps_r_ls_depth_bias;			// -0.0001f
extern ECORE_API float			ps_r_ls_squality;			// 1.0f
extern ECORE_API float			ps_r_ls_fade;				// 1.0f

extern ECORE_API float			ps_r_mblur;					// 0.5f

extern ECORE_API int			ps_r_GI_depth;				// 1..5
extern ECORE_API int			ps_r_GI_photons;			// 8..256
extern ECORE_API float			ps_r_GI_clip;				// EPS
extern ECORE_API float			ps_r_GI_refl;				// 0.9f

extern ECORE_API float			ps_r_sun_near;				// 10.0f
extern ECORE_API float			ps_r_sun_near_border;		// 1.0f
extern ECORE_API float			ps_r_sun_far;
extern ECORE_API float			ps_r_sun_depth_far_scale;	// 1.00001f
extern ECORE_API float			ps_r_sun_depth_far_bias;	// -0.0001f
extern ECORE_API float			ps_r_sun_depth_near_scale;	// 1.00001f
extern ECORE_API float			ps_r_sun_depth_near_bias;	// -0.0001f
extern ECORE_API float			ps_r_sun_lumscale;			// 0.5f
extern ECORE_API float			ps_r_sun_lumscale_hemi;		// 1.0f
extern ECORE_API float			ps_r_sun_lumscale_amb;		// 1.0f

extern ECORE_API u32			ps_GlowsPerFrame;
extern ECORE_API float			ps_r_dhemi_sky_scale;		// 1.5f
extern ECORE_API float			ps_r_dhemi_light_scale;		// 1.0f
extern ECORE_API float			ps_r_dhemi_light_flow;		// 0.1f
extern ECORE_API float			ps_r_dhemi_light_smooth;
extern ECORE_API int			ps_r_dhemi_count;			// 5

extern ECORE_API int			ps_r_wait_sleep;

extern ECORE_API Fvector3		ps_r_dof;					//	x - min (0), y - focus (1.4), z - max (100)
extern ECORE_API float			ps_r_dof_sky;				//	distance to sky
extern ECORE_API float			ps_r_dof_kernel_size;		//	7.0f

extern ECORE_API u32			ps_r_sunshafts_mode;
extern ECORE_API float			ps_r_ss_sunshafts_length;
extern ECORE_API float			ps_r_ss_sunshafts_radius;
extern ECORE_API float			ps_r_prop_ss_sample_step_phase0;
extern ECORE_API float			ps_r_prop_ss_sample_step_phase1;
//extern ECORE_API float			ps_r_prop_ss_sample_step_phase2;
extern ECORE_API float			ps_r_prop_ss_radius;
extern ECORE_API float			ps_r_prop_ss_intensity;
extern ECORE_API float			ps_r_prop_ss_blend;


extern ECORE_API Flags32 ps_r_flags;
enum RenderFlags : u32
{
	R_FLAG_SUN					= (1<<0),
	R_FLAG_SUN_FOCUS			= (1<<1),
	R_FLAG_DETAIL_BUMP			= (1<<2),
	R_FLAG_SUN_IGNORE_PORTALS	= (1<<3),
	R_FLAG_DETAIL_SHADOW		= (1<<4),
	R_FLAG_HOM_DEPTH_DRAW		= (1<<5),
	R_FLAG_MBLUR				= (1<<6),
	R_FLAG_GI					= (1<<7),
	R_FLAG_FASTBLOOM			= (1<<8),
	R_FLAG_GLOBALMATERIAL		= (1<<9),
	R_FLAG_ZFILL				= (1<<10),
	R_FLAG_R1LIGHTS				= (1<<11),

	R_FLAG_EXP_SPLIT_SCENE					= (1<<12),
	R_FLAG_EXP_DONT_TEST_UNSHADOWED			= (1<<13),
	R_FLAG_EXP_DONT_TEST_SHADOWED			= (1<<14),
	
	R_FLAG_SUN_ZCULLING			= (1<<15),
	R_FLAG_GLOW_USE				= (1<<16),
	R_FLAG_SOFT_WATER			= (1<<17),	//	Igor: need restart
	R_FLAG_SOFT_PARTICLES		= (1<<18),	//	Igor: need restart

	R_FLAG_VOLUMETRIC_LIGHTS	= (1<<19),

	R_FLAG_STEEP_PARALLAX		= (1<<20),

	R_FLAG_ACTOR_SHADOW			= (1<<22),
};

enum RFLAG_POSTSCREEN : u32
{
	R_FLAG_RAIN_DROPS			= (1 << 0),
	R_FLAG_SS_GAMMA_CORRECTION	= (1 << 1),
	R_FLAG_VIGNETTE				= (1 << 2),
	R_FLAG_CHROMATIC			= (1 << 3),
	R_FLAG_TONEMAP				= (1 << 4),
	R_FLAG_GRADING				= (1 << 5),
};

extern ECORE_API Flags32 ps_r_postscreen_flags;
extern ECORE_API Flags32 ps_r_ssao_flags;
enum
{
	R_FLAG_SSAO_BLUR			= (1<<0),
	R_FLAG_SSAO_OPT_DATA		= (1<<1),
	R_FLAG_SSAO_HALF_DATA		= (1<<2),
	R_FLAG_SSAO_HBAO			= (1<<3),
};

// Sunshafts types
enum
{
    SS_SS_OGSE,
    SS_SS_MANOWAR,
};

// Postprocess anti-aliasing types
enum
{
	FXAA = 1,
	SMAA,
	DLAA
};

// R3-specific /////////////////////////////////////////////////
extern ECORE_API u32			ps_r3_msaa;			//=	0;
extern ECORE_API xr_token		qmsaa_token[];
extern ECORE_API u32			ps_r3_msaa_atest;	//=	0;
extern ECORE_API xr_token		qmsaa_atest_token[];
extern ECORE_API u32			ps_r3_minmax_sm;	//=	0;
extern ECORE_API xr_token		qminmax_sm_token[];

extern ECORE_API float			ps_r3_dyn_wet_surf_near;	// 10.0f
extern ECORE_API float			ps_r3_dyn_wet_surf_far;		// 30.0f
extern ECORE_API int			ps_r3_dyn_wet_surf_sm_res;	// 256

extern ECORE_API Flags32 ps_r3_flags;
enum
{
	R3_FLAG_DYN_WET_SURF			= (1<<0),
	R3_FLAG_VOLUMETRIC_SMOKE		= (1<<1),
	R3_FLAG_MSAA_HYBRID				= (1<<2),
	R3_FLAG_MSAA_OPT				= (1<<3),
	//R3FLAG_MSAA					= (1<<6),
	//R3FLAG_MSAA_ALPHATEST			= (1<<7),
};

// R4-specific /////////////////////////////////////////////////
extern ECORE_API Flags32 ps_r4_flags;
enum
{
	R4_FLAG_ENABLE_TESSELLATION		= (1<<0),
	R4_FLAG_WIREFRAME				= (1<<1),
};