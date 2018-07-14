#include	"stdafx.h"
#pragma		hdrstop

#include	"xrRender_console.h"
#include	"dxRenderDeviceRender.h"

// Common
u32	ps_r_smapsize = 2048;
xr_token q_smapsize_token[] =
{
	{ "smap_1",	     1536	},
	{ "smap_2",	     2048	},
	{ "smap_2_5",	 2560	},
	{ "smap_3",		 3072	},
	{ "smap_4",      4096	},
	{ "smap_8",      8192	},
	{ "smap_16",    16384	},
	{ 0,				0	}
};

u32 ps_r_sunshafts_mode = 0;
xr_token sunshafts_mode_token[] = 
{
    { "volumetric",		SS_VOLUMETRIC	},
	{ "screen_space",	SS_SCREEN_SPACE },
#if defined(USE_DX10) || defined(USE_DX11)
	//#TODO: Refactor all render and make mrmnwar rays
#else
	{ "manowar_ssss",	SS_MANOWAR_SS },
#endif
    { 0,				0 }
};



u32 ps_Preset =	2;
xr_token qpreset_token[] =
{
	{ "Minimum",		0	},
	{ "Low",			1	},
	{ "Default",		2	},
	{ "High",			3	},
	{ "Extreme",		4	},
	{ 0,				0	}
};

u32	ps_r_ssao_mode = 1;
xr_token qssao_mode_token[] =
{
	{ "disabled",		0	},
	{ "default",		1	},
	{ "hbao",			3	},
	{ 0,				0	}
};

u32 ps_r_sun_shafts = 2;
xr_token qsun_shafts_token[] =
{
	{ "st_opt_off",		0	},
	{ "st_opt_low",		1	},
	{ "st_opt_medium",	2	},
	{ "st_opt_high",	3	},
	{ 0,				0	}
};

u32 ps_r_ssao =	3;
xr_token qssao_token[] =
{
	{ "st_opt_off",		0	},
	{ "st_opt_low",		1	},
	{ "st_opt_medium",	2	},
	{ "st_opt_high",	3	},
#if defined(USE_DX10) || defined(USE_DX11)
	{ "st_opt_ultra",	4	},
#endif
	{ 0,				0	}
};

u32 ps_r_sun_quality = 1;
xr_token qsun_quality_token[] =
{
	{ "st_opt_low",		0	},
	{ "st_opt_medium",	1	},
	{ "st_opt_high",	2	},
#if defined(USE_DX10) || defined(USE_DX11)
	{ "st_opt_ultra",	3	},
	{ "st_opt_extreme",	4	},
#endif	//	USE_DX10
	{ 0,				0	}
};

u32 ps_r_bokeh_quality = 0;
xr_token qbokeh_quality_token[] = 
{
    { "st_opt_off",		0	},
    { "st_opt_low",		1	},
    { "st_opt_medium",	2	},
    { "st_opt_high",	3	},
    { "st_opt_ultra",	4	},
    { 0,				0	}
};


int			ps_rs_loading_stages		= 0;
float		droplets_power_debug		= 0.f;

int			ps_r_Supersample			= 1;
int			ps_r_LightSleepFrames		= 10;
int			ps_r_SkeletonUpdate			= 32;
float		ps_r_pps_u					= 0.0f;
float		ps_r_pps_v					= 0.0f;
int			ps_r_fxaa					= 0;
float		ps_r_mblur					= 0.0f;
float		ps_r_gloss_factor			= 0.03f;
float		ps_r_gmaterial				= 2.2f;
float		ps_r_zfill					= 0.25f;			// .1f
int			ps_r_wait_sleep				= 0;

// Textures
int			ps_r_tf_Anisotropic			= 8;
float		ps_r_tf_Mipbias				= 0.0f;
float		ps_r_dtex_range				= 100;
// Details
float		ps_r_Detail_l_ambient		= 0.9f;
float		ps_r_Detail_l_aniso			= 0.25f;
float		ps_r_Detail_rainbow_hemi	= 0.75f;
float		ps_r_Detail_density			= 0.3f;
float		ps_r_Detail_height			= 1.0f;
int			ps_r_Detail_radius			= 49;
u32			dm_size						= 24;
u32 		dm_cache1_line				= 12;	//dm_size*2/dm_cache1_count
u32			dm_cache_line				= 49;	//dm_size+1+dm_size
u32			dm_cache_size				= 2401;	//dm_cache_line*dm_cache_line
float		dm_fade						= 47.5f;//float(2*dm_size)-.5f;
u32			dm_current_size				= 24;
u32 		dm_current_cache1_line		= 12;	//dm_current_size*2/dm_cache1_count
u32			dm_current_cache_line		= 49;	//dm_current_size+1+dm_current_size
u32			dm_current_cache_size		= 2401;	//dm_current_cache_line*dm_current_cache_line
float		dm_current_fade				= 47.5f;//float(2*dm_current_size)-.5f;
float		ps_current_detail_density	= 0.6f;
float		ps_current_detail_height	= 1.0f;
// Trees
float		ps_r_Tree_w_rot				= 10.0f;
float		ps_r_Tree_w_speed			= 1.00f;
float		ps_r_Tree_w_amp				= 0.005f;
Fvector		ps_r_Tree_Wave				= { 0.1f, 0.01f, 0.11f };
float		ps_r_Tree_SBC				= 1.5f;	// scale bias correct
// Wallmarks
float		ps_r_WallmarkTTL			= 50.f;
float		ps_r_WallmarkSHIFT			= 0.0001f;
float		ps_r_WallmarkSHIFT_V		= 0.0001f;
// LOD
float		ps_r_GLOD_ssa_start			= 256.f;
float		ps_r_GLOD_ssa_end			= 64.f;
float		ps_r_LOD					= 0.75f;
float		ps_r_ssaLOD_A				= 64.f;
float		ps_r_ssaLOD_B				= 48.f;
float		ps_r_ssaDISCARD				= 3.5f;					//RO
float		ps_r_ssaDONTSORT			= 32.f;					//RO
float		ps_r_ssaHZBvsTEX			= 96.f;					//RO
// Tonemap
float		ps_r_tonemap_middlegray		= 1.3f;
float		ps_r_tonemap_adaptation		= 10.0f;
float		ps_r_tonemap_low_lum		= 0.5f;
float		ps_r_tonemap_amount			= 1.0f;
// Light sources
float		ps_r_ls_bloom_kernel_g		= 3.0f;
float		ps_r_ls_bloom_kernel_b		= 1.0f;
float		ps_r_ls_bloom_speed			= 50.f;
float		ps_r_ls_bloom_kernel_scale	= 0.9f; // gauss
float		ps_r_ls_dsm_kernel			= 0.7f;
float		ps_r_ls_psm_kernel			= 0.7f;
float		ps_r_ls_ssm_kernel			= 0.7f;
float		ps_r_ls_bloom_threshold		= 0.03f;
float		ps_r_ls_depth_scale			= 1.00001f;			// 1.00001f
float		ps_r_ls_depth_bias			= -0.0003f;			// -0.0001f
float		ps_r_ls_squality				= 1.0f;				// 1.00f
float		ps_r_ls_fade				= 0.5f;				// 1.f
// Parallax
float		ps_r_df_parallax_h			= 0.02f;
float		ps_r_df_parallax_range		= 75.0f;
// Global illumination
int			ps_r_GI_depth				= 1;				// 1..5
int			ps_r_GI_photons				= 16;				// 8..64
float		ps_r_GI_clip				= EPS_L;			// EPS
float		ps_r_GI_refl				= 0.9f;				// .9f
// Sun
float		ps_r_sun_tsm_projection		= 0.3f;				// 0.18f
float		ps_r_sun_tsm_bias			= -0.01f;			// 
float		ps_r_sun_near				= 20.0f;			// 12.0f
float		ps_r_sun_far				= 100.0f;
float		ps_r_sun_near_border		= 0.75f;			// 1.0f
float		ps_r_sun_depth_far_scale	= 1.00000f;			// 1.00001f
float		ps_r_sun_depth_far_bias		= -0.00002f;		// -0.0000f
float		ps_r_sun_depth_near_scale	= 1.0000f;			// 1.00001f
float		ps_r_sun_depth_near_bias	= 0.00001f;			// -0.00005f
float		ps_r_sun_lumscale			= 1.0f;				// 1.0f
float		ps_r_sun_lumscale_hemi		= 1.0f;				// 1.0f
float		ps_r_sun_lumscale_amb		= 1.0f;
// Hemi lighting
float		ps_r_dhemi_sky_scale		= 0.08f;			// 1.5f
float		ps_r_dhemi_light_scale		= 0.2f;
float		ps_r_dhemi_light_flow		= 0.1f;
float		ps_r_dhemi_light_smooth		= 1.0f;				// 1.f
int			ps_r_dhemi_count			= 5;				// 5
// Depth of field
Fvector3	ps_r_dof					= Fvector3().set(-1.25f, 1.4f, 600.f); //	x - min (0), y - focus (1.4), z - max (100)
float		ps_r_dof_sky				= 30;				//	distance to sky
float		ps_r_dof_kernel_size		= 5.0f;				//	7.0f
// Sunshafts
float		ps_r_ss_sunshafts_length		= 1.0f;
float		ps_r_ss_sunshafts_radius		= 1.0f;
float		ps_r_prop_ss_radius				= 1.56f;
float		ps_r_prop_ss_sample_step_phase0	= 0.09f;
float		ps_r_prop_ss_sample_step_phase1	= 0.07f;
float		ps_r_prop_ss_blend				= 0.066f;
float		ps_r_prop_ss_intensity			= 1.0f;

Flags32	ps_r_flags = 
{ 
	R_FLAG_SUN
	| R_FLAG_EXP_DONT_TEST_UNSHADOWED 
	| R_FLAG_EXP_SPLIT_SCENE 
	| R_FLAG_DETAIL_BUMP
	| R_FLAG_SOFT_PARTICLES
	| R_FLAG_SOFT_WATER
	| R_FLAG_STEEP_PARALLAX
	| R_FLAG_SUN_FOCUS
	| R_FLAG_SUN_TSM
	| R_FLAG_TONEMAP
	| R_FLAG_MBLUR
	| R_FLAG_VOLUMETRIC_LIGHTS
};	// r2-only

Flags32	ps_r_ssao_flags = 
{
	R_FLAG_SSAO_HALF_DATA
};


// R3-specific /////////////////////////////////////////////////////
u32 ps_r3_msaa = 0;
xr_token qmsaa_token[] =
{
	{ "st_opt_off",		0	},
	{ "2x",				1	},
	{ "4x",				2	},
//	{ "8x",				3	},
	{ 0,				0	}
};

u32 ps_r3_msaa_atest = 0;
xr_token qmsaa_atest_token[] =
{
	{ "st_opt_off",					0	}, // “Off”
	{ "st_opt_atest_msaa_dx10_0",	1	}, // “DX10.0 style [Standard]”
	{ "st_opt_atest_msaa_dx10_1",	2	}, // “DX10.1 style [Higher quality]”
	{ 0,							0	}
};

u32 ps_r3_minmax_sm = 3;
xr_token qminmax_sm_token[] =
{
	{ "off",			0	},
	{ "on",				1	},
	{ "auto",			2	},
	{ "autodetect",		3	},
	{ 0,				0	}
};

float		ps_r3_dyn_wet_surf_near		= 10.0f;			// 10.0f
float		ps_r3_dyn_wet_surf_far		= 30.0f;			// 30.0f
int			ps_r3_dyn_wet_surf_sm_res	= 256;				// 256

Flags32	ps_r3_flags =
{
	R_FLAG_SSAO_HALF_DATA
	| R3_FLAG_DYN_WET_SURF
	| R3_FLAG_VOLUMETRIC_SMOKE
	//| R3_FLAG_MSAA 
	//| R3_FLAG_MSAA_OPT
	| R3_FLAG_GBUFFER_OPT
};

// R4-specific /////////////////////////////////////////////////////


Flags32	ps_r4_flags =
{
	R4_FLAG_ENABLE_TESSELLATION
};

//////////////////////////////////////////////////////////////////////////
#ifndef _EDITOR
#include	"../../xrEngine/xr_ioconsole.h"
#include	"../../xrEngine/xr_ioc_cmd.h"

#if defined(USE_DX10) || defined(USE_DX11)
#include "../xrRenderDX10/StateManager/dx10SamplerStateCache.h"
#endif	//	USE_DX10

//-----------------------------------------------------------------------
// KD
class CCC_detail_radius : public CCC_Integer
{
public:
	void apply()
	{
		dm_current_size = iFloor((float)ps_r_Detail_radius / 4) * 2;
		dm_current_cache1_line = dm_current_size * 2 / 4;		// assuming cache1_count = 4
		dm_current_cache_line = dm_current_size + 1 + dm_current_size;
		dm_current_cache_size = dm_current_cache_line*dm_current_cache_line;
		dm_current_fade = float(2 * dm_current_size) - 0.5f;
	}
	CCC_detail_radius(LPCSTR N, int* V, int _min = 0, int _max = 999) : CCC_Integer(N, V, _min, _max) { };
	virtual void Execute(LPCSTR args)
	{
		CCC_Integer::Execute(args);
		apply();
	}
	virtual void	Status(TStatus& S)
	{
		CCC_Integer::Status(S);
	}
};
// KD

class CCC_tf_Aniso : public CCC_Integer
{
public:
	void apply()
	{
		if (0 == HW.pDevice)
			return;

		int	val = *value;
		clamp(val, 1, 16);
#if defined(USE_DX10) || defined(USE_DX11)
		SSManager.SetMaxAnisotropy(val);
#else	//	USE_DX10
		for (u32 i=0; i<HW.Caps.raster.dwStages; i++)
			CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, val));
#endif	//	USE_DX10
	}
	CCC_tf_Aniso(LPCSTR N, int*	v) : CCC_Integer(N, v, 1, 16) {};
	virtual void Execute	(LPCSTR args)
	{
		CCC_Integer::Execute	(args);
		apply					();
	}
	virtual void	Status	(TStatus& S)
	{	
		CCC_Integer::Status		(S);
		apply					();
	}
};
class CCC_tf_MipBias: public CCC_Float
{
public:
	void apply()
	{
		if (0==HW.pDevice)
			return;

#if defined(USE_DX10) || defined(USE_DX11)
		SSManager.SetMipLODBias(*value);
#else	//	USE_DX10
		for (u32 i=0; i<HW.Caps.raster.dwStages; i++)
			CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) value)));
#endif	//	USE_DX10
	}

	CCC_tf_MipBias(LPCSTR N, float*	v) : CCC_Float(N, v, -3.0f, 3.0f) {};
	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute	(args);
		apply				();
	}
	virtual void	Status	(TStatus& S)
	{	
		CCC_Float::Status	(S);
		apply				();
	}
};
class CCC_R2GM : public CCC_Float
{
public:
	CCC_R2GM(LPCSTR N, float*	v) : CCC_Float(N, v, 0.f, 4.f) { *v = 0; };
	virtual void	Execute	(LPCSTR args)
	{
		if (0 == xr_strcmp(args,"on"))
			ps_r_flags.set(R_FLAG_GLOBALMATERIAL,TRUE);
		else if (0 == xr_strcmp(args,"off"))
			ps_r_flags.set(R_FLAG_GLOBALMATERIAL,FALSE);
		else 
		{
			CCC_Float::Execute(args);
			if (ps_r_flags.test(R_FLAG_GLOBALMATERIAL))
			{
				static LPCSTR	name[4]	=	{ "oren", "blin", "phong", "metal" };
				float	mid		= *value;
				int		m0		= iFloor(mid) % 4;
				int		m1		= (m0 + 1) % 4;
				float	frc		= mid - float(iFloor(mid));
				Msg		("* material set to [%s]-[%s], with lerp of [%f]",name[m0],name[m1],frc);
			}
		}
	}
};
class CCC_Screenshot : public IConsole_Command
{
public:
	CCC_Screenshot(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args)
	{
		string_path	name; name[0] = 0;
		sscanf		(args,"%s",	name);
		LPCSTR		image = xr_strlen(name)?name:0;
		::Render->Screenshot(IRender_interface::SM_NORMAL,image);
	}
};

class CCC_RestoreQuadIBData : public IConsole_Command
{
public:
	CCC_RestoreQuadIBData(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		RCache.RestoreQuadIBData();
	}
};

class CCC_ModelPoolStat : public IConsole_Command
{
public:
	CCC_ModelPoolStat(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) 
	{
		RImplementation.Models->dump();
	}
};

class CCC_SSAO_Mode : public CCC_Token
{
public:
	CCC_SSAO_Mode(LPCSTR N, u32* V, xr_token* T) : CCC_Token(N,V,T)	{};

	virtual void Execute(LPCSTR args)
	{
		CCC_Token::Execute(args);
				
		switch(*value)
		{
			case 0:
			{
				ps_r_ssao = 0;
				ps_r_ssao_flags.set(R_FLAG_SSAO_HBAO, 0);
				break;
			}
			case 1:
			{
				if (ps_r_ssao==0)
					ps_r_ssao = 1;

				ps_r_ssao_flags.set(R_FLAG_SSAO_HBAO, 0);
				ps_r_ssao_flags.set(R_FLAG_SSAO_HALF_DATA, 0);
				break;
			}
			case 2:
			{
				if (ps_r_ssao==0)
					ps_r_ssao = 1;

				ps_r_ssao_flags.set(R_FLAG_SSAO_HBAO, 0);
				ps_r_ssao_flags.set(R_FLAG_SSAO_OPT_DATA, 0);
				ps_r_ssao_flags.set(R_FLAG_SSAO_HALF_DATA, 0);
				break;
			}
			case 3:
			{
				if (ps_r_ssao==0)
					ps_r_ssao = 1;

				ps_r_ssao_flags.set(R_FLAG_SSAO_HBAO, 1);
				ps_r_ssao_flags.set(R_FLAG_SSAO_OPT_DATA, 1);
				break;
			}
		}
	}
};

//-----------------------------------------------------------------------
class CCC_Preset : public CCC_Token
{
public:
	CCC_Preset(LPCSTR N, u32* V, xr_token* T) : CCC_Token(N, V, T) {};

	virtual void Execute(LPCSTR args)
	{
		CCC_Token::Execute	(args);
		string_path	_cfg;
		string_path	cmd;
		
		switch	(*value)	
		{
			case 0:		xr_strcpy(_cfg, "rspec_minimum.ltx");	break;
			case 1:		xr_strcpy(_cfg, "rspec_low.ltx");		break;
			case 2:		xr_strcpy(_cfg, "rspec_default.ltx");	break;
			case 3:		xr_strcpy(_cfg, "rspec_high.ltx");		break;
			case 4:		xr_strcpy(_cfg, "rspec_extreme.ltx");	break;
		}
		FS.update_path(_cfg,"$game_config$",_cfg);
		strconcat(sizeof(cmd), cmd, "cfg_load", " ", _cfg);
		Console->Execute(cmd);
	}
};

#include "r__pixel_calculator.h"
class CCC_BuildSSA : public IConsole_Command
{
public:
	CCC_BuildSSA(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) 
	{
#if !defined(USE_DX10) && !defined(USE_DX11)
		//	TODO: DX10: Implement pixel calculator
		r_pixel_calculator c;
		c.run();
#endif	//	USE_DX10
	}
};

class CCC_DofFar : public CCC_Float
{
public:
	CCC_DofFar(LPCSTR N, float* V, float _min = 0.0f, float _max = 10000.0f) : CCC_Float(N, V, _min, _max) {};

	virtual void Execute(LPCSTR args) 
	{
		float v = float(atof(args));

		if (v < (ps_r_dof.y + 0.1f))
		{
			char pBuf[256];
			_snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value greater or equal to r2_dof_focus+0.1");
			Msg("~ Invalid syntax in call to '%s'", cName);
			Msg("~ Valid arguments: %s", pBuf);
			Console->Execute("r_dof_focus");
		}
		else
		{
			CCC_Float::Execute(args);
			if (g_pGamePersistent)
				g_pGamePersistent->SetBaseDof(ps_r_dof);
		}
	}

	//	CCC_Dof should save all data as well as load from config
	virtual void Save(IWriter *F) {;}
};

class CCC_DofNear : public CCC_Float
{
public:
	CCC_DofNear(LPCSTR N, float* V, float _min = 0.0f, float _max = 10000.0f) : CCC_Float(N, V, _min, _max) {};

	virtual void Execute(LPCSTR args) 
	{
		float v = float(atof(args));

		if (v > (ps_r_dof.y - 0.1f))
		{
			char pBuf[256];
			_snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value less or equal to r2_dof_focus-0.1");
			Msg("~ Invalid syntax in call to '%s'", cName);
			Msg("~ Valid arguments: %s", pBuf);
			Console->Execute("r_dof_focus");
		}
		else
		{
			CCC_Float::Execute(args);
			if (g_pGamePersistent)
				g_pGamePersistent->SetBaseDof(ps_r_dof);
		}
	}

	//	CCC_Dof should save all data as well as load from config
	virtual void Save(IWriter *F) {;}
};

class CCC_DofFocus : public CCC_Float
{
public:
	CCC_DofFocus(LPCSTR N, float* V, float _min = 0.0f, float _max = 10000.0f) : CCC_Float(N, V, _min, _max) {};

	virtual void Execute(LPCSTR args) 
	{
		float v = float(atof(args));

		if (v > (ps_r_dof.z - 0.1f))
		{
			char pBuf[256];
			_snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value less or equal to r2_dof_far-0.1");
			Msg("~ Invalid syntax in call to '%s'", cName);
			Msg("~ Valid arguments: %s", pBuf);
			Console->Execute("r_dof_far");
		}
		else if (v < (ps_r_dof.x + 0.1f))
		{
			char pBuf[256];
			_snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value greater or equal to r2_dof_far-0.1");
			Msg("~ Invalid syntax in call to '%s'", cName);
			Msg("~ Valid arguments: %s", pBuf);
			Console->Execute("r_dof_near");
		}
		else 
		{
			CCC_Float::Execute(args);
			if (g_pGamePersistent)
				g_pGamePersistent->SetBaseDof(ps_r_dof);
		}
	}

	//	CCC_Dof should save all data as well as load from config
	virtual void Save(IWriter *F) {;}
};

class CCC_Dof : public CCC_Vector3
{
public:
	CCC_Dof(LPCSTR N, Fvector* V, const Fvector _min, const Fvector _max) : CCC_Vector3(N, V, _min, _max) {};

	virtual void Execute(LPCSTR args)
	{
		Fvector v;
		if (3 != sscanf(args,"%f,%f,%f",&v.x,&v.y,&v.z))	
			InvalidSyntax(); 
		else if ((v.x > v.y - 0.1f) || (v.z < v.y + 0.1f))
		{
			InvalidSyntax();
			Msg("x <= y - 0.1");
			Msg("y <= z - 0.1");
		}
		else
		{
			CCC_Vector3::Execute(args);
			if (g_pGamePersistent)
				g_pGamePersistent->SetBaseDof(ps_r_dof);
		}
	}
	virtual void Status(TStatus& S)
	{
		xr_sprintf(S, "%f, %f, %f", value->x, value->y, value->z);
	}
	virtual void Info(TInfo& I)
	{
		xr_sprintf(I, "vector3 in range [%f,%f,%f]-[%f,%f,%f]", min.x, min.y, min.z, max.x, max.y, max.z);
	}
};

class CCC_DumpResources : public IConsole_Command
{
public:
	CCC_DumpResources(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args)
	{
		RImplementation.Models->dump();
		dxRenderDeviceRender::Instance().Resources->Dump(false);
	}
};

class CCC_SunshaftsIntensity : public CCC_Float
{
public:
	CCC_SunshaftsIntensity(LPCSTR N, float* V, float _min, float _max) : CCC_Float(N, V, _min, _max) {}
    virtual void Save(IWriter*) {}
};

// Allow real-time fog config reload
#if	(RENDER != R_R2)
#ifdef DEBUG

#include "../xrRenderDX10/3DFluid/dx103DFluidManager.h"

class CCC_Fog_Reload : public IConsole_Command
{
public:
	CCC_Fog_Reload(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) 
	{
		FluidManager.UpdateProfiles();
	}
};
#endif	//	DEBUG
#endif	//	(RENDER == R_R3) || (RENDER == R_R4)

//-----------------------------------------------------------------------
void xrRender_initconsole()
{
	CMD3(CCC_Preset,	"_preset",				&ps_Preset,	qpreset_token					);

	CMD4(CCC_Integer,	"rs_skeleton_update",	&ps_r_SkeletonUpdate,		2,		128		);
#ifdef DEBUG
	CMD1(CCC_DumpResources,	"dump_resources");
#endif

	// Common
	CMD1(CCC_Screenshot,"screenshot"			);

	// Igor: just to test bug with rain/particles corruption
	CMD1(CCC_RestoreQuadIBData,	"r_restore_quad_ib_data");
	CMD1(CCC_ModelPoolStat,"stat_models"		);

	Fvector	tw_min, tw_max;
#ifdef DEBUG
	CMD1(CCC_BuildSSA,	"build_ssa"				);
	CMD4(CCC_Integer,	"r_lsleep_frames",		&ps_r_LightSleepFrames,		4,		30		);
	CMD4(CCC_Float,		"r_ssa_glod_start",		&ps_r_GLOD_ssa_start,		128,	512		);
	CMD4(CCC_Float,		"r_ssa_glod_end",		&ps_r_GLOD_ssa_end,			16,		96		);
	CMD4(CCC_Float,		"r_wallmark_shift_pp",	&ps_r_WallmarkSHIFT,		0.0f,	1.0f	);
	CMD4(CCC_Float,		"r_wallmark_shift_v",	&ps_r_WallmarkSHIFT_V,		0.0f,	1.0f	);

	// Trees
	CMD4(CCC_Float,		"r_tree_w_amp",			&ps_r_Tree_w_amp,			0.001f,	1.0f	);
	CMD4(CCC_Float,		"r_tree_w_rot",			&ps_r_Tree_w_rot,			0.01f,	100.0f	);
	CMD4(CCC_Float,		"r_tree_w_speed",		&ps_r_Tree_w_speed,			1.0f,	10.0f	);
	tw_min.set			(EPS,EPS,EPS);
	tw_max.set			(2,2,2);
	CMD4(CCC_Vector3,	"r_tree_wave",			&ps_r_Tree_Wave,			tw_min, tw_max	);

	CMD3(CCC_Mask,		"r_use_nvdbt",			&ps_r_flags,				R_FLAG_USE_NVDBT);
	CMD3(CCC_Mask,		"rs_hom_depth_draw",	&ps_r_flags,				R_FLAG_HOM_DEPTH_DRAW	);
#endif // DEBUG

	CMD4(CCC_Float,		"r_wallmark_ttl",		&ps_r_WallmarkTTL,			1.0f,	5.f*60.f);
	CMD3(CCC_Mask,		"r_allow_r1_lights",	&ps_r_flags,				R_FLAG_R1LIGHTS	);
	CMD4(CCC_Integer,	"r_supersample",		&ps_r_Supersample,			1,		8		);
	CMD4(CCC_Integer,	"r_fxaa",				&ps_r_fxaa,					0,		1		);
	CMD2(CCC_R2GM,		"r_em",					&ps_r_gmaterial								);
	CMD4(CCC_Float,		"r_gloss_factor",		&ps_r_gloss_factor,			0.0f,	10.0f	);
	CMD4(CCC_Integer,	"r_wait_sleep",			&ps_r_wait_sleep,			0,		1		);
	CMD3(CCC_Mask,		"r_volumetric_lights",	&ps_r_flags,				R_FLAG_VOLUMETRIC_LIGHTS);
	CMD4(CCC_Float,     "r_rain_drops_power_debug",&droplets_power_debug,	0.0f, 3.0f		);

	// Shadows
	CMD3(CCC_Token,		"r_shadow_map_size",	&ps_r_smapsize,				q_smapsize_token);
	CMD3(CCC_Mask,      "r_actor_shadow",		&ps_r_flags,				R_FLAG_ACTOR_SHADOW);
	CMD3(CCC_Mask,		"r_exp_donttest_shad",	&ps_r_flags,				R_FLAG_EXP_DONT_TEST_SHADOWED);

	// LOD
	CMD4(CCC_Float,		"r_geometry_lod",		&ps_r_LOD,					0.1f,	3.0f	);
	CMD4(CCC_Float,		"r_ssa_lod_a",			&ps_r_ssaLOD_A,				16,		96		);
	CMD4(CCC_Float,		"r_ssa_lod_b",			&ps_r_ssaLOD_B,				32,		64		);

	// Details
	CMD4(CCC_Float,		"r_detail_l_ambient",	&ps_r_Detail_l_ambient,		0.5f,	0.95f	);
	CMD4(CCC_Float,		"r_detail_l_aniso",		&ps_r_Detail_l_aniso,		0.1f,	0.5f	);
	CMD4(CCC_Float,		"r_detail_height",		&ps_r_Detail_height,		1.0f,	2.0f	);
	CMD4(CCC_Float,		"r_detail_density",		&ps_r_Detail_density,		0.04f,	0.6f	);	// KD: extended from 0.2 to 0.04
	CMD4(CCC_detail_radius, "r_detail_radius",	&ps_r_Detail_radius,		49,		250		);
	CMD3(CCC_Mask,		"r_detail_shadow",		&ps_r_flags,				R_FLAG_DETAIL_SHADOW);

	// Textures
	CMD2(CCC_tf_Aniso,	"r_tf_aniso",			&ps_r_tf_Anisotropic						); //	{1..16}
	CMD2(CCC_tf_MipBias,"r_tf_mipbias",			&ps_r_tf_Mipbias							); //	{-3..3}
	CMD4(CCC_Float,		"r_dtex_range",			&ps_r_dtex_range,			5,		175		);

	// Tonemap
	CMD3(CCC_Mask,		"r_tonemap",			&ps_r_flags,				R_FLAG_TONEMAP	);
	CMD4(CCC_Float,		"r_tonemap_middlegray",	&ps_r_tonemap_middlegray,	0.0f,	2.0f	);
	CMD4(CCC_Float,		"r_tonemap_adaptation",	&ps_r_tonemap_adaptation,	0.01f,	10.0f	);
	CMD4(CCC_Float,		"r_tonemap_lowlum",		&ps_r_tonemap_low_lum,		0.0001f,1.0f	);
	CMD4(CCC_Float,		"r_tonemap_amount",		&ps_r_tonemap_amount,		0.0000f,1.0f	);

	// Light sources
	CMD3(CCC_Mask,		"r_ls_bloom_fast",		&ps_r_flags,				R_FLAG_FASTBLOOM);
	CMD4(CCC_Float,		"r_ls_bloom_kernel_scale",&ps_r_ls_bloom_kernel_scale,0.5f,	2.0f	);
	CMD4(CCC_Float,		"r_ls_bloom_kernel_g",	&ps_r_ls_bloom_kernel_g,	1.0f,	7.0f	);
	CMD4(CCC_Float,		"r_ls_bloom_kernel_b",	&ps_r_ls_bloom_kernel_b,	0.01f,	1.0f	);
	CMD4(CCC_Float,		"r_ls_bloom_threshold",	&ps_r_ls_bloom_threshold,	0.0f,	1.0f	);
	CMD4(CCC_Float,		"r_ls_bloom_speed",		&ps_r_ls_bloom_speed,		0.0f,	100.f	);
	CMD4(CCC_Float,		"r_ls_dsm_kernel",		&ps_r_ls_dsm_kernel,		0.1f,	3.0f	);
	CMD4(CCC_Float,		"r_ls_psm_kernel",		&ps_r_ls_psm_kernel,		0.1f,	3.0f	);
	CMD4(CCC_Float,		"r_ls_ssm_kernel",		&ps_r_ls_ssm_kernel,		0.1f,	3.0f	);
	CMD4(CCC_Float,		"r_ls_squality",		&ps_r_ls_squality,			0.5f,	3.0f	);
	CMD4(CCC_Float,		"r_ls_depth_scale",		&ps_r_ls_depth_scale,		0.5f,	1.5f	);
	CMD4(CCC_Float,		"r_ls_depth_bias",		&ps_r_ls_depth_bias,		-0.5f,	0.5f	);
	CMD4(CCC_Float,		"r_ls_fade",			&ps_r_ls_fade,				0.2f,	1.0f	);

	// Z-Fill
	CMD3(CCC_Mask,		"r_zfill",				&ps_r_flags,				R_FLAG_ZFILL	);
	CMD4(CCC_Float,		"r_zfill_depth",		&ps_r_zfill,				0.001f,	0.5f	);

	// Sun
	CMD3(CCC_Mask,		"r_sun",				&ps_r_flags,				R_FLAG_SUN		);
	CMD3(CCC_Token,		"r_sun_quality",		&ps_r_sun_quality,			qsun_quality_token);
	CMD3(CCC_Mask,		"r_sun_focus",			&ps_r_flags,				R_FLAG_SUN_FOCUS);
	CMD3(CCC_Mask,		"r_sun_shadow_cascede_old", &ps_r_flags,			R_FLAG_SUN_OLD);
	CMD3(CCC_Mask,		"r_sun_tsm",			&ps_r_flags,				R_FLAG_SUN_TSM	);
	CMD4(CCC_Float,		"r_sun_tsm_proj",		&ps_r_sun_tsm_projection,	0.001f,	0.8f	);
	CMD4(CCC_Float,		"r_sun_tsm_bias",		&ps_r_sun_tsm_bias,			-0.5f,	0.5f	);
	CMD4(CCC_Float,		"r_sun_near",			&ps_r_sun_near,				1.0f,	50.0f	);
	CMD4(CCC_Float,		"r_sun_far",			&ps_r_sun_far,				51.0f,	180.0f	);
	CMD4(CCC_Float,		"r_sun_near_border",	&ps_r_sun_near_border,		0.5f,	1.0f	);
	CMD4(CCC_Float,		"r_sun_depth_far_scale",&ps_r_sun_depth_far_scale,	0.5f,	1.5f	);
	CMD4(CCC_Float,		"r_sun_depth_far_bias",	&ps_r_sun_depth_far_bias,	-0.5f,	0.5f	);
	CMD4(CCC_Float,		"r_sun_depth_near_scale",&ps_r_sun_depth_near_scale,0.5f,	1.5f	);
	CMD4(CCC_Float,		"r_sun_depth_near_bias",&ps_r_sun_depth_near_bias,	-0.5f,	0.5f	);
	CMD4(CCC_Float,		"r_sun_lumscale",		&ps_r_sun_lumscale,			-1.0f,	3.0f	);
	CMD4(CCC_Float,		"r_sun_lumscale_hemi",	&ps_r_sun_lumscale_hemi,	0.0f,	3.0f	);
	CMD4(CCC_Float,		"r_sun_lumscale_amb",	&ps_r_sun_lumscale_amb,		0.0f,	3.0f	);
	
	// Motion blur
	CMD3(CCC_Mask,		"r_mblur_enabled",		&ps_r_flags,				R_FLAG_MBLUR	);
	CMD4(CCC_Float,		"r_mblur",				&ps_r_mblur,				0.0f,	1.0f	);

	// Global illumination
	CMD3(CCC_Mask,		"r_gi",					&ps_r_flags,				R_FLAG_GI);
	CMD4(CCC_Float,		"r_gi_clip",			&ps_r_GI_clip,				EPS,	0.1f	);
	CMD4(CCC_Integer,	"r_gi_depth",			&ps_r_GI_depth,				1,		5		);
	CMD4(CCC_Integer,	"r_gi_photons",			&ps_r_GI_photons,			8,		256		);
	CMD4(CCC_Float,		"r_gi_refl",			&ps_r_GI_refl,				EPS_L,	0.99f	);

#ifndef MASTER_GOLD
	// Hemi lighting
	CMD4(CCC_Integer,	"r_dhemi_count",		&ps_r_dhemi_count,			4,		25		);
	CMD4(CCC_Float,		"r_dhemi_sky_scale",	&ps_r_dhemi_sky_scale,		0.0f,	100.0f	);
	CMD4(CCC_Float,		"r_dhemi_light_scale",	&ps_r_dhemi_light_scale,	0.0f,	100.0f	);
	CMD4(CCC_Float,		"r_dhemi_light_flow",	&ps_r_dhemi_light_flow,		0.0f,	1.0f	);
	CMD4(CCC_Float,		"r_dhemi_smooth",		&ps_r_dhemi_light_smooth,	0.0f,	10.0f	);

	CMD3(CCC_Mask,		"r_sun_shadow_cascede_zcul",&ps_r_flags,			R_FLAG_SUN_ZCULLING		);
#endif

	// Depth of field
    CMD3(CCC_Token,     "r_dof_quality",				&ps_r_bokeh_quality,	qbokeh_quality_token);
	tw_min.set(-10000,-10000,0); tw_max.set(10000,10000,10000);
	CMD4(CCC_Dof,		"r_dof",						&ps_r_dof,				tw_min, tw_max		);
	CMD4(CCC_DofNear,	"r_dof_near",					&ps_r_dof.x,			tw_min.x, tw_max.x	);
	CMD4(CCC_DofFocus,	"r_dof_focus",					&ps_r_dof.y,			tw_min.y, tw_max.y	);
	CMD4(CCC_DofFar,	"r_dof_far",					&ps_r_dof.z,			tw_min.z, tw_max.z	);
	CMD4(CCC_Float,		"r_dof_kernel",					&ps_r_dof_kernel_size,	0.0f,	10.f		);
	CMD4(CCC_Float,		"r_dof_sky",					&ps_r_dof_sky,			-10000.f,10000.f	);
	
	// Ambient occlusion
	CMD3(CCC_Token,		"r_ssao",						&ps_r_ssao,				qssao_token			);
	CMD3(CCC_SSAO_Mode,	"r_ssao_mode",					&ps_r_ssao_mode,		qssao_mode_token	);
	CMD3(CCC_Mask,		"r_ssao_blur",					&ps_r_ssao_flags,		R_FLAG_SSAO_BLUR	);		//Need restart
	CMD3(CCC_Mask,		"r_ssao_opt_data",				&ps_r_ssao_flags,		R_FLAG_SSAO_OPT_DATA);		//Need restart
	CMD3(CCC_Mask,		"r_ssao_half_data",				&ps_r_ssao_flags,		R_FLAG_SSAO_HALF_DATA);		//Need restart
	CMD3(CCC_Mask,		"r_ssao_hbao",					&ps_r_ssao_flags,		R_FLAG_SSAO_HBAO	);		//Need restart

	// Sunshafts
	CMD3(CCC_Token,		"r_sun_shafts",					&ps_r_sun_shafts,		qsun_shafts_token		);
    CMD3(CCC_Token,		"r_sunshafts_mode",				&ps_r_sunshafts_mode,	sunshafts_mode_token	);
    CMD4(CCC_Float,		"r_ss_sunshafts_length",		&ps_r_ss_sunshafts_length,			0.2f,	1.5f);
    CMD4(CCC_Float,		"r_ss_sunshafts_radius",		&ps_r_ss_sunshafts_radius,			0.5f,	2.f	);
	CMD4(CCC_Float,		"r_SunShafts_SampleStep_Phase1",&ps_r_prop_ss_sample_step_phase0,	0.01f,	0.2f);
	CMD4(CCC_Float,		"r_SunShafts_SampleStep_Phase2",&ps_r_prop_ss_sample_step_phase1,	0.01f,	0.2f);
	CMD4(CCC_Float,		"r_SunShafts_Radius",			&ps_r_prop_ss_radius,				0.5f,	2.0f);
	CMD4(CCC_Float,		"r_SunShafts_Intensity",		&ps_r_prop_ss_intensity,			0.0f,	2.0f);
	CMD4(CCC_Float,		"r_SunShafts_Blend",			&ps_r_prop_ss_blend,				0.01f,	1.0f);
	CMD4(CCC_SunshaftsIntensity, "r_sunshafts_intensity",&ps_r_sunshafts_intensity,			0.0f,	1.0f);

	// Bump
	CMD3(CCC_Mask,		"r_steep_parallax",				&ps_r_flags,			R_FLAG_STEEP_PARALLAX);
	CMD4(CCC_Float,		"r_parallax_h",					&ps_r_df_parallax_h,	0.0f,	0.5f	);
	CMD3(CCC_Mask,		"r_detail_bump",				&ps_r_flags,			R_FLAG_DETAIL_BUMP);

	// Soft-
	CMD3(CCC_Mask,		"r_soft_water",					&ps_r_flags,			R_FLAG_SOFT_WATER);
	CMD3(CCC_Mask,		"r_soft_particles",				&ps_r_flags,			R_FLAG_SOFT_PARTICLES);

	// R3-specific /////////////////////////////////////////////////////

	// MSAA
	CMD3(CCC_Token,		"r3_msaa",						&ps_r3_msaa,				qmsaa_token);
	CMD3(CCC_Mask,		"r3_gbuffer_opt",				&ps_r3_flags,				R3_FLAG_GBUFFER_OPT);
	CMD3(CCC_Mask,		"r3_use_dx10_1",				&ps_r3_flags,				R3_FLAG_USE_DX10_1);
	CMD3(CCC_Token,		"r3_msaa_alphatest",			&ps_r3_msaa_atest,			qmsaa_atest_token);
	CMD3(CCC_Token,		"r3_minmax_sm",					&ps_r3_minmax_sm,			qminmax_sm_token);
	// Dynamic wet surfaces
	CMD3(CCC_Mask,		"r3_dynamic_wet_surfaces",		&ps_r3_flags,				R3_FLAG_DYN_WET_SURF);
	CMD4(CCC_Float,		"r3_dynamic_wet_surfaces_near",	&ps_r3_dyn_wet_surf_near,	10,	70		);
	CMD4(CCC_Float,		"r3_dynamic_wet_surfaces_far",	&ps_r3_dyn_wet_surf_far,	30,	100		);
	CMD4(CCC_Integer,	"r3_dynamic_wet_surfaces_sm_res",&ps_r3_dyn_wet_surf_sm_res,64,	2048	);
	// Volumetric smoke
	CMD3(CCC_Mask,		"r3_volumetric_smoke",			&ps_r3_flags,			R3_FLAG_VOLUMETRIC_SMOKE);

	// R4-specific /////////////////////////////////////////////////////
	CMD3(CCC_Mask,		"r4_enable_tessellation",		&ps_r4_flags,		R4_FLAG_ENABLE_TESSELLATION); // Need restart
	CMD3(CCC_Mask,		"r4_wireframe",					&ps_r4_flags,		R4_FLAG_WIREFRAME); // Need restart




	// Allow real-time fog config reload
#if	(RENDER == R_R3) || (RENDER == R_R4)
#ifdef	DEBUG
	CMD1(CCC_Fog_Reload,"r3_fog_reload");
#endif
#endif
}
#endif // _EDITOR
