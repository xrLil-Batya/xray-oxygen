#include "stdafx.h"
#pragma hdrstop

#pragma warning(push)
#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(pop)

#include "ResourceManager.h"
#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

#include "../../xrEngine/igame_persistent.h"
#include "../../xrEngine/environment.h"
#include "../../xrEngine/DirectXMathExternal.h"

#include "dxRenderDeviceRender.h"
#include "../../xrEngine/Rain.h"

// matrices
#define	BIND_DECLARE(xf)	\
class cl_xform_##xf	: public R_constant_setup {	virtual void setup (R_constant* C) { RCache.xforms.set_c_##xf (C); } }; \
	static cl_xform_##xf	binder_##xf
BIND_DECLARE(w);
BIND_DECLARE(invw);
BIND_DECLARE(v);
BIND_DECLARE(invv);
BIND_DECLARE(p);
BIND_DECLARE(invp);
BIND_DECLARE(wv);
BIND_DECLARE(vp);
BIND_DECLARE(wvp);

#define DECLARE_TREE_BIND(c)	\
	class cl_tree_##c: public R_constant_setup	{virtual void setup(R_constant* C) {RCache.tree.set_c_##c(C);} };	\
	static cl_tree_##c	tree_binder_##c

DECLARE_TREE_BIND(m_xform_v);
DECLARE_TREE_BIND(m_xform);
DECLARE_TREE_BIND(consts);
DECLARE_TREE_BIND(wave);
DECLARE_TREE_BIND(wind);
DECLARE_TREE_BIND(c_scale);
DECLARE_TREE_BIND(c_bias);
DECLARE_TREE_BIND(c_sun);

static class cl_hemi_cube_pos_faces : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		RCache.hemi.set_c_pos_faces(C);
	}
} binder_hemi_cube_pos_faces;

static class cl_hemi_cube_neg_faces : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		RCache.hemi.set_c_neg_faces(C);
	}
} binder_hemi_cube_neg_faces;

static class cl_material : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		RCache.hemi.set_c_material(C);
	}
} binder_material;

static class cl_texgen : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		Fmatrix mTexgen;
#ifdef USE_DX11
		Fmatrix mTexelAdjust = 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				1.0f,			0.0f,
			0.5f,				0.5f,				0.0f,			1.0f
		};
#else
		float _w	= float(RDEVICE.dwWidth);
		float _h	= float(RDEVICE.dwHeight);
		float o_w	= (0.5f / _w);
		float o_h	= (0.5f / _h);
		Fmatrix mTexelAdjust = 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				1.0f,			0.0f,
			0.5f + o_w,			0.5f + o_h,			0.0f,			1.0f
		};
#endif
		mTexgen.mul(mTexelAdjust, RCache.xforms.m_wvp);
		RCache.set_c(C, mTexgen);
	}
} binder_texgen;

static class cl_VPtexgen : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		Fmatrix mTexgen;
#ifdef USE_DX11
		Fmatrix mTexelAdjust = 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				1.0f,			0.0f,
			0.5f,				0.5f,				0.0f,			1.0f
		};
#else
		float _w	= float(RDEVICE.dwWidth);
		float _h	= float(RDEVICE.dwHeight);
		float o_w	= (0.5f / _w);
		float o_h	= (0.5f / _h);
		Fmatrix mTexelAdjust = 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				1.0f,			0.0f,
			0.5f + o_w,			0.5f + o_h,			0.0f,			1.0f
		};
#endif
		mTexgen.mul(mTexelAdjust,RCache.xforms.m_vp);
		RCache.set_c(C, mTexgen);
	}
} binder_VPtexgen;

// fog
#ifndef _EDITOR
static class cl_fog_plane : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		if (marker!=Device.dwFrame)
		{
			// Plane
			Fvector4 plane;
			Fmatrix& M		= Device.mFullTransform;
			plane.x			= -(M._14 + M._13);
			plane.y			= -(M._24 + M._23);
			plane.z			= -(M._34 + M._33);
			plane.w			= -(M._44 + M._43);
			float denom		= -1.0f / _sqrt(_sqr(plane.x) + _sqr(plane.y) + _sqr(plane.z));
			plane.mul		(denom);

			// Near/Far
			float A			= Environment().CurrentEnv->fog_near;
			float B			= 1.0f / (Environment().CurrentEnv->fog_far - A);
			result.set		(-plane.x*B, -plane.y*B, -plane.z*B, 1.0f - (plane.w - A)*B); // view-plane
		}
		RCache.set_c(C,result);
	}
} binder_fog_plane;

// fog-params
static class cl_fog_params : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			// Near/Far
			float n = Environment().CurrentEnv->fog_near;
			float f = Environment().CurrentEnv->fog_far;
			float r = 1.0f / (f - n);
			result.set(-n * r, n, f, r);
		}
		RCache.set_c(C, result);
	}
} binder_fog_params;

// fog-color
static class cl_fog_color : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			CEnvDescriptor&	desc = *Environment().CurrentEnv;
			result.set(desc.fog_color.x, desc.fog_color.y, desc.fog_color.z, desc.fog_density);
		}
		RCache.set_c(C, result);
	}
} binder_fog_color;

static class cl_wind_params : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			CEnvDescriptor&	E = *Environment().CurrentEnv;
			result.set(E.wind_direction, E.wind_velocity, 0.0f, 0.0f);
		}
		RCache.set_c(C, result);
	}
} binder_wind_params;
#endif

// times
static class cl_times : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		float t = RDEVICE.fTimeGlobal;
		RCache.set_c(C, t, t * 10.0f, t / 10.0f, _sin(t));
	}
} binder_times;

// eye-params
static class cl_eye_P : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		Fvector& V = RDEVICE.vCameraPosition;
		RCache.set_c(C, V.x, V.y, V.z, 1.0f);
	}
} binder_eye_P;

// eye-params
static class cl_eye_D : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		Fvector& V = RDEVICE.vCameraDirection;
		RCache.set_c(C, V.x, V.y, V.z, 0.0f);
	}
} binder_eye_D;

// eye-params
static class cl_eye_N : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		Fvector& V = RDEVICE.vCameraTop;
		RCache.set_c(C, V.x, V.y, V.z, 0.0f);
	}
} binder_eye_N;

#ifndef _EDITOR
// D-Light0
static class cl_sun0_color : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			CEnvDescriptor&	desc = *Environment().CurrentEnv;
			result.set(desc.sun_color.x, desc.sun_color.y, desc.sun_color.z, 0.0f);
		}
		RCache.set_c(C, result);
	}
} binder_sun0_color;

static class cl_sun0_dir_w : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			CEnvDescriptor&	desc = *Environment().CurrentEnv;
			result.set(desc.sun_dir.x, desc.sun_dir.y, desc.sun_dir.z, 0.0f);
		}
		RCache.set_c(C, result);
	}
} binder_sun0_dir_w;

static class cl_sun0_dir_e : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			Fvector D;
			CEnvDescriptor&	desc = *Environment().CurrentEnv;
			Device.mView.transform_dir(D, desc.sun_dir);
			D.normalize();
			result.set(D.x, D.y, D.z, 0);
		}
		RCache.set_c(C, result);
	}
} binder_sun0_dir_e;

// xyz = ambient color, w = env descriptor weight
static class cl_amb_color : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			CEnvDescriptorMixer& desc = *Environment().CurrentEnv;
			result.set(desc.ambient.x, desc.ambient.y, desc.ambient.z, desc.weight);
		}
		RCache.set_c(C, result);
	}
} binder_amb_color;

static class cl_hemi_color : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			CEnvDescriptor&	desc = *Environment().CurrentEnv;
			result.set(desc.hemi_color.x, desc.hemi_color.y, desc.hemi_color.z, desc.hemi_color.w);
		}
		RCache.set_c(C, result);
	}
} binder_hemi_color;
#endif

static class cl_ssao : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		float fSSAONoise = 2.0f;
		fSSAONoise *= tan(deg2rad(67.5f));
		fSSAONoise /= tan(deg2rad(Device.fFOV));

		float fSSAOKernelSize = 150.0f;
		fSSAOKernelSize *= tan(deg2rad(67.5f));
		fSSAOKernelSize /= tan(deg2rad(Device.fFOV));

		RCache.set_c(C, fSSAONoise, fSSAOKernelSize, 0.0f, 0.0f);
	}
} binder_ssao;

static class cl_screen_res : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		RCache.set_c(C, (float)RDEVICE.dwWidth, (float)RDEVICE.dwHeight, 1.0f / (float)RDEVICE.dwWidth, 1.0f / (float)RDEVICE.dwHeight);
	}
} binder_screen_res;

static class cl_screen_params : public R_constant_setup
{
	Fvector4 result;
	virtual void setup(R_constant* C)
	{
		float fov		= float(Device.fFOV);
		float aspect	= float(Device.fASPECT);
		result.set(fov, aspect, tan(deg2rad(fov) / 2.0f), Environment().CurrentEnv->far_plane*0.75f);
		RCache.set_c(C, result);
	}
} binder_screen_params;

static class cl_rain_params : public R_constant_setup
{
    u32 marker;
    Fvector4 result;
    virtual void setup(R_constant* C)
	{
		float wetnessW		= Environment().eff_Rain->GetWorldWetness();
		float wetnessCVE	= Environment().eff_Rain->GetCurrViewEntityWetness();
		float rainDensity	= Environment().CurrentEnv->rain_density;

        RCache.set_c(C, wetnessW, wetnessCVE, rainDensity, 0.0f);
    }
} binder_rain_params;

static class cl_parallax : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		extern float r_dtex_range;

		float h = ps_r_df_parallax_h;
		RCache.set_c(C, h, -h / 2.f, 1.f / r_dtex_range, 1.f / r_dtex_range);
	}
}	binder_parallax;

static class cl_tree_amplitude_intensity : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		CEnvDescriptor&	E = *Environment().CurrentEnv;
		float fValue = E.m_fTreeAmplitudeIntensity;
		RCache.set_c(C, fValue, fValue, fValue, 0);
	}
} binder_tree_amplitude_intensity;

static class cl_pos_decompress_params : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		float VertTan = -1.0f * tanf(deg2rad(Device.fFOV / 2.0f));
		float HorzTan = -VertTan / Device.fASPECT;

		RCache.set_c(C, HorzTan, VertTan, (2.0f * HorzTan) / (float)Device.dwWidth, (2.0f * VertTan) / (float)Device.dwHeight);

	}
} binder_pos_decompress_params;

static class cl_water_intensity : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		CEnvDescriptor&	E = *Environment().CurrentEnv;
		float fValue = E.m_fWaterIntensity;
		RCache.set_c(C, fValue, fValue, fValue, 0.0f);
	}
} binder_water_intensity;

static class cl_sun_shafts_intensity : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		CEnvDescriptor&	E = *Environment().CurrentEnv;
		float fValue = E.m_fSunShaftsIntensity;
		RCache.set_c(C, fValue, fValue, fValue, 0.0f);
	}
} binder_sun_shafts_intensity;

static class cl_collimator_state : public R_constant_setup
{
	virtual void setup(R_constant* C) 
	{ 
		RCache.set_c(C, g_pGamePersistent->m_pGShaderConstants.collimator_mark);
	}
} binder_collimator_state;

#ifdef USE_DX11
static class cl_alpha_ref : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		StateManager.BindAlphaRef(C);
	}
} binder_alpha_ref;

#ifdef USE_DX11
static class cl_LOD : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		RCache.LOD.set_LOD(C);
	}
} binder_LOD;
#endif
#endif


// Standart constant-binding
//MatthewKush to all: make sure byte alignment is correct. Same with structs.
void CBlender_Compile::SetMapping()
{
    r_Constant("ogse_c_screen", &binder_screen_params);

	// collimator
	r_Constant("m_collimator_state", &binder_collimator_state);

	// Matrices
	r_Constant				("m_W",				&binder_w);
	r_Constant				("m_invW",			&binder_invw);
	r_Constant				("m_V",				&binder_v);
	r_Constant				("m_invV",			&binder_invv);
	r_Constant				("m_P",				&binder_p);
	r_Constant				("m_invP",			&binder_invp);
	r_Constant				("m_WV",			&binder_wv);
	r_Constant				("m_VP",			&binder_vp);
	r_Constant				("m_WVP",			&binder_wvp);

	r_Constant				("m_xform_v",		&tree_binder_m_xform_v);
	r_Constant				("m_xform",			&tree_binder_m_xform);
	r_Constant				("consts",			&tree_binder_consts);
	r_Constant				("wave",			&tree_binder_wave);
	r_Constant				("wind",			&tree_binder_wind);
	r_Constant				("c_scale",			&tree_binder_c_scale);
	r_Constant				("c_bias",			&tree_binder_c_bias);
	r_Constant				("c_sun",			&tree_binder_c_sun);

	// Hemi cube
	r_Constant				("L_material",			&binder_material);
	r_Constant				("hemi_cube_pos_faces",	&binder_hemi_cube_pos_faces);
	r_Constant				("hemi_cube_neg_faces",	&binder_hemi_cube_neg_faces);

	// Igor: temp solution for the texgen functionality in the shader
	r_Constant				("m_texgen",		&binder_texgen);
	r_Constant				("mVPTexgen",		&binder_VPtexgen);

	// Environment parameters ////////////////////////////////////////////
	// Global lighting
	r_Constant				("L_sun_color",		&binder_sun0_color);
	r_Constant				("L_sun_dir_w",		&binder_sun0_dir_w);
	r_Constant				("L_sun_dir_e",		&binder_sun0_dir_e);
	r_Constant				("L_hemi_color",	&binder_hemi_color);
	r_Constant				("L_ambient",		&binder_amb_color);
	// Fog
	r_Constant				("fog_plane",		&binder_fog_plane);
	r_Constant				("fog_params",		&binder_fog_params);
	r_Constant				("fog_color",		&binder_fog_color);
	// Wind
	r_Constant				("wind_params",		&binder_wind_params);
	// Rain
	r_Constant				("rain_params",		&binder_rain_params);
	// Water
	r_Constant				("water_intensity", &binder_water_intensity);
	// Sunshafts
	r_Constant				("sun_shafts_intensity", &binder_sun_shafts_intensity);
	// Sunshafts
	r_Constant				("ssao_params", &binder_ssao);
	///////////////////////////////////////////////////////////////////////

	// Time
	r_Constant				("timers",			&binder_times);

	// Eye parameters
	r_Constant				("eye_position",	&binder_eye_P);
	r_Constant				("eye_direction",	&binder_eye_D);
	r_Constant				("eye_normal",		&binder_eye_N);

	// Parallax
	r_Constant				("parallax",		&binder_parallax);

	// Screen parameters
	r_Constant				("screen_res",					&binder_screen_res);
	r_Constant				("pos_decompression_params",	&binder_pos_decompress_params);

	r_Constant				("m_AlphaRef",		&binder_alpha_ref);
	r_Constant				("triLOD",			&binder_LOD);

	// detail
	//if (bDetail	&& detail_scaler)
	//	Igor: bDetail can be overridden by no_detail_texture option.
	//	But shader can be deatiled implicitly, so try to set this parameter
	//	anyway.
	if (detail_scaler)
		r_Constant			("dt_params",		detail_scaler);

	// other common
	for (u32 it = 0; it < DEV->v_constant_setup.size(); it++)
	{
		std::pair<shared_str, R_constant_setup*> cs = DEV->v_constant_setup[it];
		r_Constant(*cs.first, cs.second);
	}
}
