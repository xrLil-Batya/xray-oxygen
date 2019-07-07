#include "stdafx.h"
#include "../xrRender/resourcemanager.h"
#include "blender_light_occq.h"
#include "blender_light_mask.h"
#include "blender_light_direct.h"
#include "blender_light_point.h"
#include "blender_light_spot.h"
#include "blender_light_reflected.h"
#include "blender_combine.h"
#include "blender_ssao.h"
#include "dx11MinMaxSMBlender.h"
#include "../xrRenderDX10/msaa/dx10MSAABlender.h"
#include "../xrRenderDX10/DX10 Rain/dx10RainBlender.h"

#include "../xrRender/dxRenderDeviceRender.h"
#include "../../xrEngine/std_classes.h"
#include "../../xrEngine/Spectre/Spectre.h"
#include <D3DX10Tex.h>
RENDER_API CRenderTarget* pRenderTarget = nullptr;

void	CRenderTarget::u_setrt(const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, ID3DDepthStencilView* zb)
{
	VERIFY(_1 || zb);
	if (_1)
	{
		dwWidth = _1->dwWidth;
		dwHeight = _1->dwHeight;
	}
	else
	{
		D3D_DEPTH_STENCIL_VIEW_DESC	desc;
		zb->GetDesc(&desc);

		if (!RImplementation.o.dx10_msaa)
			VERIFY(desc.ViewDimension == D3D_DSV_DIMENSION_TEXTURE2D);

		ID3DResource *pRes;

		zb->GetResource(&pRes);

		ID3DTexture2D *pTex = (ID3DTexture2D *)pRes;

		D3D_TEXTURE2D_DESC	TexDesc;

		pTex->GetDesc(&TexDesc);

		// size of 2d texture (simple texture)
		dwWidth = TexDesc.Width;
		dwHeight = TexDesc.Height;
		///////////////////////////////////////////
		// after all we release ID3DResource
		_RELEASE(pRes);
	}
	///////////////////////////////////////////
	if (_1) 
		RCache.set_RT(_1->pRT, 0); 
	else 
		RCache.set_RT(NULL, 0);
	///////////////////////////////////////////
	if (_2) 
		RCache.set_RT(_2->pRT, 1); 
	else 
		RCache.set_RT(NULL, 1);
	///////////////////////////////////////////
	if (_3) 
		RCache.set_RT(_3->pRT, 2); 
	else 
		RCache.set_RT(NULL, 2);
	///////////////////////////////////////////
	RCache.set_ZB(zb);
}

void	CRenderTarget::u_setrt(const ref_rt& _1, const ref_rt& _2, ID3DDepthStencilView* zb)
{
	VERIFY(_1 || zb);
	if (_1)
	{
		dwWidth = _1->dwWidth;
		dwHeight = _1->dwHeight;
	}
	else
	{
		D3D_DEPTH_STENCIL_VIEW_DESC	desc;
		zb->GetDesc(&desc);
		if (!RImplementation.o.dx10_msaa)
			VERIFY(desc.ViewDimension == D3D_DSV_DIMENSION_TEXTURE2D);

		ID3DResource *pRes;

		zb->GetResource(&pRes);

		ID3DTexture2D *pTex = (ID3DTexture2D *)pRes;

		D3D_TEXTURE2D_DESC	TexDesc;

		pTex->GetDesc(&TexDesc);

		dwWidth = TexDesc.Width;
		dwHeight = TexDesc.Height;
		_RELEASE(pRes);
	}
	///////////////////////////////////////////
	if (_1) 
		RCache.set_RT(_1->pRT, 0); 
	else 
		RCache.set_RT(NULL, 0);
	///////////////////////////////////////////
	if (_2) 
		RCache.set_RT(_2->pRT, 1); 
	else 
		RCache.set_RT(NULL, 1);
	///////////////////////////////////////////
	RCache.set_ZB(zb);
}

void	CRenderTarget::u_setrt(u32 W, u32 H, ID3DRenderTargetView* _1, ID3DRenderTargetView* _2, ID3DRenderTargetView* _3, ID3DDepthStencilView* zb)
{
	dwWidth = W;
	dwHeight = H;
	RCache.set_RT(_1, 0);
	RCache.set_RT(_2, 1);
	RCache.set_RT(_3, 2);
	RCache.set_ZB(zb);
}

// 2D texgen (texture adjustment matrix)
void	CRenderTarget::u_compute_texgen_screen(Fmatrix& m_Texgen)
{
	Fmatrix			m_TexelAdjust =
	{
		0.5f,				0.0f,				0.0f,			0.0f,
		0.0f,				-0.5f,				0.0f,			0.0f,
		0.0f,				0.0f,				1.0f,			0.0f,
		//	Removing half pixel offset
		0.5f,				0.5f ,				0.0f,			1.0f
	};
	m_Texgen.mul(m_TexelAdjust, RCache.xforms.m_wvp);
}

// 2D texgen for jitter (texture adjustment matrix)
void	CRenderTarget::u_compute_texgen_jitter(Fmatrix&		m_Texgen_J)
{
	// place into	0..1 space
	Fmatrix			m_TexelAdjust =
	{
		0.5f,				0.0f,				0.0f,			0.0f,
		0.0f,				-0.5f,				0.0f,			0.0f,
		0.0f,				0.0f,				1.0f,			0.0f,
		0.5f,				0.5f,				0.0f,			1.0f
	};
	m_Texgen_J.mul(m_TexelAdjust, RCache.xforms.m_wvp);

	// rescale - tile it
	///////////////////////////////////////////
	float scale_X = float(Device.dwWidth) / float(TEX_jitter);
	float scale_Y = float(Device.dwHeight) / float(TEX_jitter);
	///////////////////////////////////////////
	m_TexelAdjust.scale(scale_X, scale_Y, 1.f);
	m_Texgen_J.mulA_44(m_TexelAdjust);
}

u8 fpack(float v) 
{
	s32	_v = iFloor(((v + 1)*.5f)*255.f + .5f);
	clamp(_v, 0, 255);
	return	u8(_v);
}
u8 fpackZ(float v) 
{
	s32	_v = iFloor(_abs(v)*255.f + .5f);
	clamp(_v, 0, 255);
	return	u8(_v);
}
Fvector	vunpack(s32 x, s32 y, s32 z) {
	Fvector	pck;
	pck.x = (float(x) / 255.f - .5f)*2.f;
	pck.y = (float(y) / 255.f - .5f)*2.f;
	pck.z = -float(z) / 255.f;
	return	pck;
}
Fvector	vunpack(Ivector src) {
	return	vunpack(src.x, src.y, src.z);
}
Ivector	vpack(Fvector src)
{
	Fvector			_v;
	int	bx = fpack(src.x);
	int by = fpack(src.y);
	int bz = fpackZ(src.z);
	// dumb test
	float	e_best = flt_max;
	int		r = bx, g = by, b = bz;
#ifdef DEBUG
	int		d = 0;
#else
	int		d = 3;
#endif
	for (int x = std::max(bx - d, 0); x <= std::min(bx + d, 255); x++)
		for (int y = std::max(by - d, 0); y <= std::min(by + d, 255); y++)
			for (int z = std::max(bz - d, 0); z <= std::min(bz + d, 255); z++)
			{
				_v = vunpack(x, y, z);
				float	m = _v.magnitude();
				float	me = _abs(m - 1.f);
				if (me>0.03f)	continue;
				_v.div(m);
				float	e = _abs(src.dotproduct(_v) - 1.f);
				if (e<e_best) {
					e_best = e;
					r = x, g = y, b = z;
				}
			}
	Ivector		ipck;
	ipck.set(r, g, b);
	return		ipck;
}

void	generate_jitter(DWORD*	dest, u32 elem_count)
{
	const	int		cmax = 8;
	svector<Ivector2, cmax>		samples;
	while (samples.size()<elem_count * 2)
	{
		Ivector2	test;
		test.set(::Random.randI(0, 256), ::Random.randI(0, 256));
		BOOL		valid = TRUE;
		for (u32 t = 0; t<samples.size(); t++)
		{
			int		dist = _abs(test.x - samples[t].x) + _abs(test.y - samples[t].y);
			if (dist<32) {
				valid = false;
				break;
			}
		}
		if (valid)	samples.push_back(test);
	}
	for (u32 it = 0; it<elem_count; it++, dest++)
		*dest = color_rgba(samples[2 * it].x, samples[2 * it].y, samples[2 * it + 1].y, samples[2 * it + 1].x);
}

CRenderTarget::CRenderTarget()
{
	CLS_ID = CLSID_RENDER;
	SpectreObjectId = SpectreEngineClient::CreateProxyObject(this);

	pRenderTarget = this;
	u32 SampleCount = 1;

	if (ps_r_ssao_mode != 2)
		ps_r_ssao = std::min(ps_r_ssao, (u32)3);

	RImplementation.o.ssao_ultra = ps_r_ssao > 3;
	if (RImplementation.o.dx10_msaa)
		SampleCount = RImplementation.o.dx10_msaa_samples;

	param_blur = 0.f;
	param_gray = 0.f;
	param_noise = 0.f;
	param_duality_h = 0.f;
	param_duality_v = 0.f;
	param_noise_fps = 25.f;
	param_noise_scale = 1.f;

	im_noise_time = 1 / 100;
	im_noise_shift_w = 0;
	im_noise_shift_h = 0;

	param_color_base = color_rgba(127, 127, 127, 0);
	param_color_gray = color_rgba(85, 85, 85, 0);
	param_color_add.set(0.0f, 0.0f, 0.0f);

	dwAccumulatorClearMark = 0;

	// Blenders
	b_occq							= xr_new<CBlender_light_occq>			();
	b_accum_mask					= xr_new<CBlender_accum_direct_mask>	();
	b_accum_direct					= xr_new<CBlender_accum_direct>			();
	b_accum_point					= xr_new<CBlender_accum_point>			();
	b_accum_spot					= xr_new<CBlender_accum_spot>			();
	b_accum_reflected				= xr_new<CBlender_accum_reflected>		();
	b_combine						= xr_new<CBlender_combine>				();
	b_ssao							= xr_new<CBlender_SSAO_noMSAA>			();

	if (RImplementation.o.dx10_msaa)
	{
		int bound = RImplementation.o.dx10_msaa_samples;

		if (RImplementation.o.dx10_msaa_opt)
			bound = 1;

		for (int i = 0; i < bound; ++i)
		{
			static LPCSTR SampleDefs[] = { "0","1","2","3","4","5","6","7" };
			b_combine_msaa[i]					= xr_new<CBlender_combine_msaa>();
			b_accum_mask_msaa[i]				= xr_new<CBlender_accum_direct_mask_msaa>();
			b_accum_direct_msaa[i]				= xr_new<CBlender_accum_direct_msaa>();
			b_accum_direct_volumetric_msaa[i]	= xr_new<CBlender_accum_direct_volumetric_msaa>();
			b_accum_spot_msaa[i]				= xr_new<CBlender_accum_spot_msaa>();
			b_accum_volumetric_msaa[i]			= xr_new<CBlender_accum_volumetric_msaa>();
			b_accum_point_msaa[i]				= xr_new<CBlender_accum_point_msaa>();
			b_accum_reflected_msaa[i]			= xr_new<CBlender_accum_reflected_msaa>();
			b_ssao_msaa[i]						= xr_new<CBlender_SSAO_MSAA>();

			static_cast<CBlender_accum_direct_mask_msaa*>		(b_accum_mask_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
			static_cast<CBlender_accum_direct_volumetric_msaa*>	(b_accum_direct_volumetric_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
			static_cast<CBlender_accum_direct_msaa*>			(b_accum_direct_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
			static_cast<CBlender_accum_volumetric_msaa*>		(b_accum_volumetric_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
			static_cast<CBlender_accum_spot_msaa*>				(b_accum_spot_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
			static_cast<CBlender_accum_point_msaa*>				(b_accum_point_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
			static_cast<CBlender_accum_reflected_msaa*>			(b_accum_reflected_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
			static_cast<CBlender_combine_msaa*>					(b_combine_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
			static_cast<CBlender_SSAO_MSAA*>					(b_ssao_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
		}
	}
	//	NORMAL
	{
		u32		w = Device.dwWidth, h = Device.dwHeight;
                //TODO: Recreate position with s_depth, use striclty as new z-buffer.
		//Then we'll have a new RGBA16F gbuffer slot open :) We'd pack what was previously in
		//s_position in there, and maybe some new stuff.
		//You'll actually save performance with a third RT bound to GBUFFER.
		//This is why on modern machines turning "r3_gbuffer_opt off" can be faster.
		rt_Position.create					(r2_RT_P, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, SampleCount);
        //rt_Depth.create(r2_RT_depth, w, h, D3DFMT_D24S8, SampleCount); //not needed for depth prepass..

		if (RImplementation.o.dx10_msaa)
			rt_MSAADepth.create				(r2_RT_MSAAdepth, w, h, DXGI_FORMAT_D24_UNORM_S8_UINT, SampleCount);

		// select albedo & accum
		if (RImplementation.o.mrtmixdepth)
		{
			// NV50
			rt_Color.create					(r2_RT_albedo, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, SampleCount);
			rt_Accumulator.create			(r2_RT_accum, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, SampleCount);
		}
		else
		{
			// can't - mix-depth
			// NV40
			rt_Color.create			(r2_RT_albedo, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, SampleCount);	// expand to full
			rt_Accumulator.create	(r2_RT_accum, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, SampleCount);
			//rt_Normal.create(r2_RT_N, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, SampleCount);

#if 0
//Suggested new Gbuffer
rt_Depth.create(r2_RT_depth, w, h, D3DFMT_D24S8, SampleCount); //z-buffer for depth
rt_Color.create(r2_RT_albedo, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, SampleCount); //Pack with 8bit Color, 8-bit gloss in Alpha

//AND 8-bit Normals, int (or uint) material-type, 7-bit (right?) hemi (do we even need to reconstruct it?)
//Currently won't work: game uses both Normals and "Normal Error map" equaling 2 * 8bit RGB.
//Someone would need to recreate all games normal maps, either in DXT5NM (compatible with DX9) OR
//BC5 format (best quality, but DX11 only, can't use on DX10 or DX9)

//then, if you need to store more, you have two open Gbuffer (actually 3 more is supported by D3D11) slots :) 
//could be used for PBR among many other things.
#endif
		}

// Note: We should re-add the "RImplementation.o..) because otherwise these textures
// are loaded even when not used. Same with the rain.
        // Mrmnwar SunShaft Screen Space
//		if (RImplementation.o.sunshaft_mrmnwar)
        {
            rt_SunShaftsMask.create			(r2_RT_SunShaftsMask,			w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
            rt_SunShaftsMaskSmoothed.create	(r2_RT_SunShaftsMaskSmoothed,	w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
            rt_SunShaftsPass0.create		(r2_RT_SunShaftsPass0,			w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
            s_ssss_mrmnwar.create			("effects\\ss_sunshafts_mrmnwar");
        }

        // RT - KD Screen space sunshafts
//		if (RImplementation.o.sunshaft_screenspace)
        {
            rt_sunshafts_0.create			(r2_RT_sunshafts0, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
            rt_sunshafts_1.create			(r2_RT_sunshafts1, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
            s_ssss_ogse.create				("effects\\ss_sunshafts_ogse");
        }

		// generic(LDR) RTs
		rt_Generic_0.create					(r2_RT_generic0, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, 1);
		rt_Generic_1.create					(r2_RT_generic1, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, 1);
		rt_Generic_2.create					(r2_RT_generic2, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, 1);
		rt_Generic.create					(r2_RT_generic, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, 1);

		// Second viewport
		rt_secondVP.create					(r2_RT_secondVP, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, 1);
		rt_OXY_holomarks.create				(r2_OXY_holo_mark); // конкретно с этим мы и будем работать далее,
																// заменяя ссылку на текстуру с нужной маркой
		m_MarkTexture						= "";

		if (RImplementation.o.dx10_msaa)
		{
			rt_Generic_0_r.create			(r2_RT_generic0_r, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, SampleCount);
			rt_Generic_1_r.create			(r2_RT_generic1_r, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, SampleCount);
		}

		// For higher quality blends
		rt_Volumetric.create				(r2_RT_volumetric, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, SampleCount);
	}

	// OCCLUSION
	s_occq.create							(b_occq, "r2\\occq");

	// Puddles
	s_water.create							("effects\\puddles", "water\\water_water");

	// DIRECT (spot)
	DXGI_FORMAT depth_format					= (DXGI_FORMAT)RImplementation.o.HW_smap_FORMAT;

	u32	size								= RImplementation.o.smapsize;
	rt_smap_depth.create					(r2_RT_smap_depth, size, size, depth_format);

	if (RImplementation.o.dx10_minmax_sm)
	{
		rt_smap_depth_minmax.create			(r2_RT_smap_depth_minmax,	size/4,size/4, DXGI_FORMAT_R32_FLOAT);
		CBlender_createminmax TempBlender;
		s_create_minmax_sm.create			(&TempBlender, "null");
	}
	s_accum_mask.create						(b_accum_mask, "r3\\accum_mask");
	s_accum_direct.create					(b_accum_direct, "r3\\accum_direct");

	if(RImplementation.o.dx10_msaa)
	{
		int bound = RImplementation.o.dx10_msaa_samples;

		if( RImplementation.o.dx10_msaa_opt )
			bound = 1;

		for( int i = 0; i < bound; ++i )
		{
			s_accum_direct_msaa[i].create	(b_accum_direct_msaa[i], "r3\\accum_direct");
			s_accum_mask_msaa[i].create		(b_accum_mask_msaa[i], "r3\\accum_direct");
		}
	}

	s_accum_direct_volumetric.create	("accum_volumetric_sun_nomsaa");

	if (RImplementation.o.dx10_minmax_sm)
		s_accum_direct_volumetric_minmax.create("accum_volumetric_sun_nomsaa_minmax");

	if (RImplementation.o.dx10_msaa)
	{
		static LPCSTR snames[] = { 
			"accum_volumetric_sun_msaa0",
			"accum_volumetric_sun_msaa1",
			"accum_volumetric_sun_msaa2",
			"accum_volumetric_sun_msaa3",
			"accum_volumetric_sun_msaa4",
			"accum_volumetric_sun_msaa5",
			"accum_volumetric_sun_msaa6",
			"accum_volumetric_sun_msaa7" };

		int bound = RImplementation.o.dx10_msaa_samples;

		if( RImplementation.o.dx10_msaa_opt )
				bound = 1;

		for( int i = 0; i < bound; ++i )
			s_accum_direct_volumetric_msaa[i].create		(snames[i]);
	}

	//	RAIN
	//	TODO: DX10: Create resources only when DX10 rain is enabled.
	//	Or make DX10 rain switch dynamic?
	{
		CBlender_rain	TempBlender;
		s_rain.create(&TempBlender, "null");

		if (RImplementation.o.dx10_msaa)
		{
			static LPCSTR SampleDefs[] = { "0","1","2","3","4","5","6","7" };
			CBlender_rain_msaa	TempBlenderMSAA[8];

			int bound = RImplementation.o.dx10_msaa_samples;

			if (RImplementation.o.dx10_msaa_opt)
				bound = 1;

			for (u32 i = 0; i < (u32)bound; ++i)
			{
				TempBlenderMSAA[i].SetDefine	("ISAMPLE", SampleDefs[i]);
				s_rain_msaa[i].create		(&TempBlenderMSAA[i], "null");
				s_accum_spot_msaa[i].create	(b_accum_spot_msaa[i], "r2\\accum_spot_s", "lights\\lights_spot01");
				s_accum_point_msaa[i].create(b_accum_point_msaa[i], "r2\\accum_point_s");
				s_accum_volume_msaa[i].create(b_accum_volumetric_msaa[i], "lights\\lights_spot01");
				s_combine_msaa[i].create	(b_combine_msaa[i], "r2\\combine");
			}
		}
	}

	if (RImplementation.o.dx10_msaa)
	{
		CBlender_msaa TempBlender;

		s_mark_msaa_edges.create(&TempBlender, "null");
	}

	// POINT
	{
		s_accum_point.create			(b_accum_point, "r2\\accum_point_s");
		accum_point_geom_create			();
		g_accum_point.create			(D3DFVF_XYZ, g_accum_point_vb, g_accum_point_ib);
		accum_omnip_geom_create			();
		g_accum_omnipart.create			(D3DFVF_XYZ, g_accum_omnip_vb, g_accum_omnip_ib);
	}

	// SPOT
	{
		s_accum_spot.create				(b_accum_spot, "r2\\accum_spot_s", "lights\\lights_spot01");
		accum_spot_geom_create			();
		g_accum_spot.create				(D3DFVF_XYZ, g_accum_spot_vb, g_accum_spot_ib);
	}

	{
		s_accum_volume.create			("accum_volumetric", "lights\\lights_spot01");
		accum_volumetric_geom_create	();
		g_accum_volumetric.create		(D3DFVF_XYZ, g_accum_volumetric_vb, g_accum_volumetric_ib);
	}


	// REFLECTED
	{
		s_accum_reflected.create		(b_accum_reflected, "r2\\accum_refl");
		if (RImplementation.o.dx10_msaa)
		{
			int bound = RImplementation.o.dx10_msaa_samples;

			if (RImplementation.o.dx10_msaa_opt)
				bound = 1;

			for (int i = 0; i < bound; ++i)
			{
				s_accum_reflected_msaa[i].create(b_accum_reflected_msaa[i], "null");
			}
		}
	}

	// BLOOM
	{
		/////////////////////////////////////////
		DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;	
		/////////////////////////////////////////
		u32	w = BLOOM_size_X, h = BLOOM_size_Y;
		u32 fvf_build = D3DFVF_XYZRHW | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3);
		u32 fvf_filter = (u32)D3DFVF_XYZRHW | D3DFVF_TEX8 | D3DFVF_TEXCOORDSIZE4(0) | D3DFVF_TEXCOORDSIZE4(1) | D3DFVF_TEXCOORDSIZE4(2) | D3DFVF_TEXCOORDSIZE4(3) | D3DFVF_TEXCOORDSIZE4(4) | D3DFVF_TEXCOORDSIZE4(5) | D3DFVF_TEXCOORDSIZE4(6) | D3DFVF_TEXCOORDSIZE4(7);
		/////////////////////////////////////////
		rt_Bloom_1.create			(r2_RT_bloom1, BLOOM_size_X, BLOOM_size_Y, fmt);
		rt_Bloom_2.create			(r2_RT_bloom2, BLOOM_size_X, BLOOM_size_Y, fmt);
		/////////////////////////////////////////
		g_bloom_build.create		(fvf_build, RCache.Vertex.Buffer(), RCache.QuadIB);
		g_bloom_filter.create		(fvf_filter, RCache.Vertex.Buffer(), RCache.QuadIB);
		/////////////////////////////////////////
		s_bloom_dbg_1.create		("effects\\screen_set", r2_RT_bloom1);
		s_bloom_dbg_2.create		("effects\\screen_set", r2_RT_bloom2);
		/////////////////////////////////////////
		s_bloom.create				("effects\\bloom_build");
		f_bloom_factor				= 0.5f;
	}

	// TONEMAP
	{
		rt_LUM_64.create			(r2_RT_luminance_t64, 64, 64, DXGI_FORMAT_R16G16B16A16_FLOAT);
		rt_LUM_8.create				(r2_RT_luminance_t8, 8, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
		s_luminance.create			("effects\\bloom_luminance");
		f_luminance_adapt			= 0.5f;

		t_LUM_src.create			(r2_RT_luminance_src);
		t_LUM_dest.create			(r2_RT_luminance_cur);

		// create pool
		for (u32 it = 0; it < HW.Caps.iGPUNum * 2; it++)
		{
			string256 name;
			xr_sprintf(name, "%s_%d", r2_RT_luminance_pool, it);
			rt_LUM_pool[it].create(name, 1, 1, DXGI_FORMAT_R32_FLOAT);
			FLOAT ColorRGBA[4] = { 127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f };
			HW.pContext->ClearRenderTargetView(rt_LUM_pool[it]->pRT, ColorRGBA);
		}
		u_setrt(Device.dwWidth, Device.dwHeight, HW.pBaseRT, nullptr, nullptr, HW.pBaseZB);
	}

	// HBAO
	if (RImplementation.o.ssao_opt_data)
	{
		u32 w = 0;
		u32 h = 0;
		if (RImplementation.o.ssao_half_data)
		{
			w = Device.dwWidth / 2;
			h = Device.dwHeight / 2;
		}
		else
		{
			w = Device.dwWidth;
			h = Device.dwHeight;
		}

		DXGI_FORMAT	fmt = HW.Caps.id_vendor == 0x10DE ? DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R16_FLOAT;
		rt_half_depth.create			(r2_RT_half_depth, w, h, fmt);

		s_ssao.create(b_ssao, "r2\\ssao");
	}

	// COMBINE
	{
		static D3DVERTEXELEMENT9 dwDecl[] =
		{
			{ 0, 0,  D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },	// pos+uv
			D3DDECL_END()
		};
		s_combine.create				(b_combine, "r2\\combine");
		s_combine_volumetric.create		("combine_volumetric");
		s_combine_dbg_0.create			("effects\\screen_set", r2_RT_smap_surf);
		s_combine_dbg_1.create			("effects\\screen_set", r2_RT_luminance_t8);
		s_combine_dbg_Accumulator.create("effects\\screen_set", r2_RT_accum);
		/////////////////////////////////////////
		g_combine_VP.create				(dwDecl, RCache.Vertex.Buffer(), RCache.QuadIB);
		g_combine.create				(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
		g_combine_2UV.create			(FVF::F_TL2uv, RCache.Vertex.Buffer(), RCache.QuadIB);
		g_combine_cuboid.create			(dwDecl, RCache.Vertex.Buffer(), RCache.Index.Buffer());
		/////////////////////////////////////////
		u32 fvf_aa_AA					= D3DFVF_XYZRHW | D3DFVF_TEX7 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3) | D3DFVF_TEXCOORDSIZE2(4) | D3DFVF_TEXCOORDSIZE4(5) | D3DFVF_TEXCOORDSIZE4(6);
		g_aa_AA.create					(fvf_aa_AA, RCache.Vertex.Buffer(), RCache.QuadIB);
		/////////////////////////////////////////
		t_envmap_0.create				(r2_T_envs0);
		t_envmap_1.create				(r2_T_envs1);
	}

	// Gamma correction 
	{
		// RT, used as look up table
		rt_GammaLUT.create			(r2_RT_gamma_lut, 256, 1, DXGI_FORMAT_R8G8B8A8_UNORM);
		s_gamma.create				("effects\\pp_gamma");
	}

	// Post combine_2 effects:
	// - Antialiasing
	// - Rain droplets
	// - Vignette

	// SMAA RTs
	{
		u32	w = Device.dwWidth;
		u32 h = Device.dwHeight;

//		rt_prev_frame0.create	(r2_RT_prev_frame0,		w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
		rt_smaa_edgetex.create	(r2_RT_smaa_edgetex,	w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
		rt_smaa_blendtex.create	(r2_RT_smaa_blendtex,	w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	if (RImplementation.o.dx10_msaa)
	{
		s_pp_antialiasing.create("effects\\pp_antialiasing_msaa");
		s_pp_taa.create			("effects\\taa_msaa");
		s_rain_drops.create		("effects\\screen_rain_droplets_msaa");
		s_postscreen.create		("effects\\postscreen_msaa");
	}
	else
	{
		s_pp_antialiasing.create("effects\\pp_antialiasing");
		s_pp_taa.create			("effects\\taa");
		s_rain_drops.create		("effects\\screen_rain_droplets");
		s_postscreen.create		("effects\\postscreen");
	}

	// Build textures
	{
		// Build material(s)
		{
			// Create immutable texture. 
			// So we need to init data _before_ the creation.
			u16	tempData[TEX_material_LdotN*TEX_material_LdotH*TEX_material_Count];

			D3D_TEXTURE3D_DESC desc;
			std::memset(&desc, 0, sizeof(D3D_TEXTURE3D_DESC));
			desc.Width			= TEX_material_LdotN;
			desc.Height			= TEX_material_LdotH;
			desc.Depth			= TEX_material_Count;
			desc.MipLevels		= 1;
			desc.Format			= DXGI_FORMAT_R8G8_UNORM;
			desc.Usage			= D3D_USAGE_IMMUTABLE;
			desc.BindFlags		= D3D_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags		= 0;

			D3D_SUBRESOURCE_DATA subData;
			subData.pSysMem				= tempData;
			subData.SysMemPitch			= desc.Width*2;
			subData.SysMemSlicePitch	= desc.Height*subData.SysMemPitch;

			// Fill it (addr: x=dot(L,N),y=dot(L,H))
			for (u32 slice = 0; slice < TEX_material_Count; ++slice)
			{
				for (u32 y = 0; y < TEX_material_LdotH; ++y)
				{
					for (u32 x = 0; x < TEX_material_LdotN; ++x)
					{
						u16* p = (u16*)(LPBYTE(subData.pSysMem) + slice * subData.SysMemSlicePitch + y * subData.SysMemPitch + x * 2);
						float ld = float(x) / float(TEX_material_LdotN - 1);
						float ls = float(y) / float(TEX_material_LdotH - 1) + EPS_S;
						ls *= powf(ld, 1.0f / 32.0f);
						float fd, fs = 1.0f; // (ls * 1.01f)^0

						switch (slice)
						{
						case 0: // Looks like OrenNayar
						{
							fd = powf(ld, 0.75f);		// 0.75
							fs = powf(ls, 16.0f)*0.5f;
						}	break;
						case 1: // Looks like Blinn
						{
							fd = powf(ld, 0.90f);		// 0.90
							fs = powf(ls, 24.0f);
						}	break;
						case 2: // Looks like Phong
						{
							fd = ld;					// 1.0
							fs = powf(ls*1.01f, 128.0f);
						}	break;
						case 3: // Looks like Metal
						{
							float s0 = _abs(1.0f - _abs(0.05f*_sin(33.0f*ld) + ld - ls));
							float s1 = _abs(1.0f - _abs(0.05f*_cos(33.0f*ld*ls) + ld - ls));
							float s2 = _abs(1.0f - _abs(ld - ls));
							fd = ld;					// 1.0
							fs = powf(std::max(std::max(s0, s1), s2), 24.0f);
							fs *= powf(ld, 1.0f / 7.0f);
						}	break;
						default:
							fd = fs = 0;
						}
						s32 _d = clampr(iFloor(fd*255.5f), 0, 255);
						s32 _s = clampr(iFloor(fs*255.5f), 0, 255);
						if ((y == (TEX_material_LdotH - 1)) && (x == (TEX_material_LdotN - 1)))
						{
							_d = 255;
							_s = 255;
						}
						*p = u16(_s * 256 + _d);
					}
				}
			}

			R_CHK(HW.pDevice->CreateTexture3D(&desc, &subData, &t_material_surf));
			t_material = dxRenderDeviceRender::Instance().Resources->_CreateTexture(r2_material);
			t_material->surface_set(t_material_surf);
		}

		// Build noise table
		{
			static const int sampleSize = 4;
			D3D_SUBRESOURCE_DATA subData[TEX_jitter_count];

			// Generic jitter textures
			{
				u32	tempData[TEX_jitter_count][TEX_jitter*TEX_jitter];

				D3D_TEXTURE2D_DESC desc;
				std::memset(&desc, 0, sizeof(D3D_TEXTURE2D_DESC));
				desc.Width				= TEX_jitter;
				desc.Height				= TEX_jitter;
				desc.MipLevels			= 1;
				desc.ArraySize			= 1;
				desc.SampleDesc.Count	= 1;
				desc.SampleDesc.Quality = 0;
				desc.Format				= DXGI_FORMAT_R8G8B8A8_SNORM;
				desc.Usage				= D3D_USAGE_DEFAULT;
				desc.BindFlags			= D3D_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags		= 0;
				desc.MiscFlags			= 0;
		
				for (int it = 0; it < TEX_jitter_count - 1; ++it)
				{
					subData[it].pSysMem = tempData[it];
					subData[it].SysMemPitch = desc.Width * sampleSize;
				}

				// Fill it
				for (u32 y = 0; y < TEX_jitter; ++y)
				{
					for (u32 x = 0; x < TEX_jitter; ++x)
					{
						DWORD data[TEX_jitter_count - 1];
						generate_jitter(data, TEX_jitter_count - 1);
						for (u32 it = 0; it < TEX_jitter_count - 1; ++it)
						{
							u32* p = (u32*)(LPBYTE(subData[it].pSysMem) + y * subData[it].SysMemPitch + x * 4);
							*p = data[it];
						}
					}
				}

				for (int it = 0; it < TEX_jitter_count - 1; ++it)
				{
					string_path name;
					xr_sprintf(name, "%s%d", r2_jitter, it);
					R_CHK(HW.pDevice->CreateTexture2D(&desc, &subData[it], &t_noise_surf[it]));
					t_noise[it] = dxRenderDeviceRender::Instance().Resources->_CreateTexture(name);
					t_noise[it]->surface_set(t_noise_surf[it]);
				}
			}

			// HBAO jitter texture
			{
				float tempDataHBAO[TEX_jitter*TEX_jitter * 4];
				D3D_TEXTURE2D_DESC descHBAO;
				std::memset(&descHBAO, 0, sizeof(D3D_TEXTURE2D_DESC));
				descHBAO.Width				= TEX_jitter;
				descHBAO.Height				= TEX_jitter;
				descHBAO.MipLevels			= 1;
				descHBAO.ArraySize			= 1;
				descHBAO.SampleDesc.Count	= 1;
				descHBAO.SampleDesc.Quality = 0;
				descHBAO.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;
				descHBAO.Usage				= D3D_USAGE_DEFAULT;
				descHBAO.BindFlags			= D3D_BIND_SHADER_RESOURCE;
				descHBAO.CPUAccessFlags		= 0;
				descHBAO.MiscFlags			= 0;

				const auto it = TEX_jitter_count - 1;
				subData[it].pSysMem = tempDataHBAO;
				subData[it].SysMemPitch = descHBAO.Width * sampleSize * sizeof(float);

				// Fill it
				for (u32 y = 0; y < TEX_jitter; ++y)
				{
					for (u32 x = 0; x < TEX_jitter; ++x)
					{
						float numDir = 1.0f;
						switch (ps_r_ssao)
						{
						case 1: numDir = 4.0f; break;
						case 2: numDir = 6.0f; break;
						case 3: numDir = 8.0f; break;
						case 4: numDir = 8.0f; break;
						}
						float angle = 2 * PI * ::Random.randF(0.0f, 1.0f) / numDir;
						float dist = ::Random.randF(0.0f, 1.0f);

						float *p = (float*)(LPBYTE(subData[it].pSysMem) + y * subData[it].SysMemPitch + x * 4 * sizeof(float));

						*p = (float)(_cos(angle));
						*(p + 1) = (float)(_sin(angle));
						*(p + 2) = (float)(dist);
						*(p + 3) = 0;
					}
				}

				string_path name;
				xr_sprintf(name, "%s%d", r2_jitter, it);
				R_CHK(HW.pDevice->CreateTexture2D(&descHBAO, &subData[it], &t_noise_surf[it]));
				t_noise[it] = dxRenderDeviceRender::Instance().Resources->_CreateTexture(name);
				t_noise[it]->surface_set(t_noise_surf[it]);
			}

			// Mipped jitter
			{
				D3D_TEXTURE2D_DESC desc;
				std::memset(&desc, 0, sizeof(D3D_TEXTURE2D_DESC));
				desc.Width				= TEX_jitter;
				desc.Height				= TEX_jitter;
				desc.MipLevels			= 0; // Autogen mipmaps
				desc.ArraySize			= 1;
				desc.SampleDesc.Count	= 1;
				desc.SampleDesc.Quality = 0;
				desc.Format				= DXGI_FORMAT_R8G8B8A8_SNORM;
				desc.Usage				= D3D_USAGE_DEFAULT;
				desc.BindFlags			= D3D_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags		= 0;
				desc.MiscFlags			= 0;

				R_CHK(HW.pDevice->CreateTexture2D(&desc, 0, &t_noise_surf_mipped));
				t_noise_mipped = dxRenderDeviceRender::Instance().Resources->_CreateTexture(r2_jitter_mipped);
				t_noise_mipped->surface_set(t_noise_surf_mipped);

				// Update texture. Generate mips.
				HW.pContext->CopySubresourceRegion(t_noise_surf_mipped, 0, 0, 0, 0, t_noise_surf[0], 0, nullptr);

				D3DX11FilterTexture(HW.pContext, t_noise_surf_mipped, 0, D3DX10_FILTER_POINT);
			}
		
			// HQ noise
			{
				u32 w = Device.dwWidth, h = Device.dwHeight;

				D3D_TEXTURE2D_DESC desc;
				std::memset(&desc, 0, sizeof(D3D_TEXTURE2D_DESC));
				desc.Width				= w;
				desc.Height				= h;
				desc.MipLevels			= 1;
				desc.ArraySize			= 1;
				desc.SampleDesc.Count	= 1;
				desc.SampleDesc.Quality = 0;
				desc.Format				= DXGI_FORMAT_R8G8B8A8_SNORM;
				desc.Usage				= D3D_USAGE_DEFAULT;
				desc.BindFlags			= D3D_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags		= 0;
				desc.MiscFlags			= 0;

				u32* tempData = (u32*)Memory.mem_alloc(w * h * sizeof(u32));
				D3D_SUBRESOURCE_DATA subData1;
				std::memset(&subData1, 0, sizeof(D3D_SUBRESOURCE_DATA));
				subData1.pSysMem = tempData;
				subData1.SysMemPitch = desc.Width * sampleSize;

				// Fill it
				for (u32 y = 0; y < h; ++y)
				{
					for (u32 x = 0; x < w; ++x)
					{
						DWORD data;
						generate_jitter(&data, 1);
						u32* p = (u32*)(LPBYTE(subData1.pSysMem) + y * subData1.SysMemPitch + x * 4);
						*p = data;
					}
				}

				R_CHK(HW.pDevice->CreateTexture2D(&desc, &subData1, &t_noise_hq_surf));
				t_noise_hq = dxRenderDeviceRender::Instance().Resources->_CreateTexture(r2_jitter_hq);
				t_noise_hq->surface_set(t_noise_hq_surf);

				Memory.mem_free(tempData);
			}
		}
	}

	// PP
	s_postprocess.create				("effects\\postprocess");
	g_postprocess.create				(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX3,RCache.Vertex.Buffer(),RCache.QuadIB);
	if (RImplementation.o.dx10_msaa)
		s_postprocess_msaa.create		("effects\\postprocess_msaa");

	// Menu
	s_menu.create						("effects\\ui_distort");
	g_menu.create						(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);

	// 
	dwWidth								= Device.dwWidth;
	dwHeight							= Device.dwHeight;
}

CRenderTarget::~CRenderTarget()
{
	// Textures
	t_material->surface_set		(nullptr);

#ifdef DEBUG
	_SHOW_REF					("t_material_surf",t_material_surf);
#endif
	_RELEASE					(t_material_surf);

	t_LUM_src->surface_set		(nullptr);
	t_LUM_dest->surface_set		(nullptr);

#ifdef DEBUG
	ID3DBaseTexture* pSurf = nullptr;

	pSurf = t_envmap_0->surface_get();
	if (pSurf)
		pSurf->Release();
	_SHOW_REF("t_envmap_0 - #small",pSurf);

	pSurf = t_envmap_1->surface_get();
	if (pSurf)
		pSurf->Release();
	_SHOW_REF("t_envmap_1 - #small",pSurf);
#endif // DEBUG
	t_envmap_0->surface_set		(nullptr);
	t_envmap_1->surface_set		(nullptr);
	t_envmap_0.destroy			();
	t_envmap_1.destroy			();

	// Jitter
	for (int it = 0; it < TEX_jitter_count; it++)
	{
		t_noise[it]->surface_set(nullptr);
#ifdef DEBUG
		_SHOW_REF("t_noise_surf[it]", t_noise_surf[it]);
#endif
		_RELEASE(t_noise_surf[it]);
	}

	t_noise_mipped->surface_set(nullptr);
#ifdef DEBUG
	_SHOW_REF("t_noise_surf_mipped",t_noise_surf_mipped);
#endif
	_RELEASE(t_noise_surf_mipped);

	// Destroy HQ noise texture
	t_noise_hq->surface_set(nullptr);
	_RELEASE(t_noise_hq_surf);

	// Light geoms
	accum_spot_geom_destroy		();
	accum_omnip_geom_destroy	();
	accum_point_geom_destroy	();
	accum_volumetric_geom_destroy();

	// Blenders
	xr_delete(b_combine);
	xr_delete(b_accum_reflected);
	xr_delete(b_accum_spot);
	xr_delete(b_accum_point);
	xr_delete(b_accum_direct);
	xr_delete(b_ssao);

	if (RImplementation.o.dx10_msaa)
	{
		int bound = RImplementation.o.dx10_msaa_samples;
		if (RImplementation.o.dx10_msaa_opt)
			bound = 1;

		for (int i = 0; i < bound; ++i)
		{
			xr_delete(b_combine_msaa[i]);
			xr_delete(b_accum_direct_msaa[i]);
			xr_delete(b_accum_mask_msaa[i]);
			xr_delete(b_accum_direct_volumetric_msaa[i]);
			xr_delete(b_accum_spot_msaa[i]);
			xr_delete(b_accum_volumetric_msaa[i]);
			xr_delete(b_accum_point_msaa[i]);
			xr_delete(b_accum_reflected_msaa[i]);
			xr_delete(b_ssao_msaa[i]);
		}
	}
	xr_delete(b_accum_mask);
	xr_delete(b_occq);
}

void CRenderTarget::reset_light_marker(bool bResetStencil)
{
	dwLightMarkerID = 5;
	if (bResetStencil)
	{
		u32		Offset;
		float	_w						= float(Device.dwWidth);
		float	_h						= float(Device.dwHeight);
		u32		C						= color_rgba(255, 255, 255, 255);
		float	eps						= 0;
		float	_dw						= 0.5f;
		float	_dh						= 0.5f;
		FVF::TL* pv						= (FVF::TL*) RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
		pv->set							(-_dw, _h - _dh, eps, 1.f, C, 0, 0);	pv++;
		pv->set							(-_dw, -_dh, eps, 1.f, C, 0, 0);	pv++;
		pv->set							(_w - _dw, _h - _dh, eps, 1.f, C, 0, 0);	pv++;
		pv->set							(_w - _dw, -_dh, eps, 1.f, C, 0, 0);	pv++;
		RCache.Vertex.Unlock			(4, g_combine->vb_stride);
		RCache.set_Element				(s_occq->E[2]);
		RCache.set_Geometry				(g_combine);
		RCache.Render					(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	}
}

void CRenderTarget::increment_light_marker()
{
	dwLightMarkerID += 2;

	const u32 iMaxMarkerValue = RImplementation.o.dx10_msaa ? 127 : 255;

	if (dwLightMarkerID > iMaxMarkerValue)
		reset_light_marker(true);
}

bool CRenderTarget::need_to_render_sunshafts()
{
	if (ps_r_sun_shafts == 0)
		return false;

	light* sun = (light*)RImplementation.Lights.sun._get();
	CEnvDescriptor&	E = *Environment().CurrentEnv;
	Fcolor sun_color = sun->color;
	float fValue = E.m_fSunShaftsIntensity * Diffuse::u_diffuse2s(sun_color.r, sun_color.g, sun_color.b);
	if (fValue < EPS)
		return false;

	return true;
}

bool CRenderTarget::use_minmax_sm_this_frame()
{
	switch (RImplementation.o.dx10_minmax_sm)
	{
	case CRender::MMSM_ON:
		return true;
	case CRender::MMSM_AUTO:
		return need_to_render_sunshafts();
	case CRender::MMSM_AUTODETECT:
	{
		u32 dwScreenArea = HW.m_ChainDesc.BufferDesc.Width * HW.m_ChainDesc.BufferDesc.Height;

		if ((dwScreenArea >= RImplementation.o.dx10_minmax_sm_screenarea_threshold))
			return need_to_render_sunshafts();
		else
			return false;
	}

	default:
		return false;
	}

}

void CRenderTarget::RenderScreenQuad(u32 w, u32 h, ID3DRenderTargetView* rt, ref_selement &sh, xr_unordered_map<LPCSTR, Fvector4*>* consts)
{
	u32 Offset	= 0;
	float d_Z	= EPS_S;
	float d_W	= 1.0f;
	u32	C		= color_rgba(0, 0, 0, 255);

	if (rt)
		u_setrt(w, h, rt, nullptr, nullptr, HW.pBaseZB);

	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(false);
 
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, 0, 1); pv++;
	pv->set(0, 0, d_Z, d_W, C, 0, 0); pv++;
	pv->set(w, h, d_Z, d_W, C, 1, 1); pv++;
	pv->set(w, 0, d_Z, d_W, C, 1, 0); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

    RCache.set_Element(sh);
	if (consts)
	{
		for (const auto &C : *consts)
			RCache.set_c(C.first, *C.second);
	}
    RCache.set_Geometry	(g_combine);
    RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::RenderScreenQuad(u32 w, u32 h, ref_rt &rt, ref_selement &sh, xr_unordered_map<LPCSTR, Fvector4*>* consts)
{
	RenderScreenQuad(w, h, rt ? rt->pRT : nullptr, sh, consts);
}
