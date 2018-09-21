#include "stdafx.h"
#include "../../xrEngine/igame_persistent.h"
#include "../../xrEngine/environment.h"
#include "r2_puddles.h"
#include "../xrRender/dxEnvironmentRender.h"
#include "../xrRender/dxRenderDeviceRender.h"

#define STENCIL_CULL 0

float hclip(float v, float dim)
{
	return 2.0f * v / dim - 1.0f;
}

void CRenderTarget::phase_combine()
{
	bool bMenuPP = g_pGamePersistent ? g_pGamePersistent->OnRenderPPUI_query() : false;

	u32 Offset = 0;
	Fvector2 p0, p1;

	//*** exposure-pipeline
	u32 gpu_id = Device.dwFrame%HW.Caps.iGPUNum;
	if (Device.m_SecondViewport.IsSVPActive())	//+SecondVP+ Fix for screen flickering
	{
		gpu_id = (Device.dwFrame - 1) % HW.Caps.iGPUNum;	// Фикс "мерцания" tonemapping (HDR) после выключения двойного рендера. 
															// Побочный эффект - при работе двойного рендера скорость изменения tonemapping (HDR) падает в два раза
															// Мерцание связано с тем, что HDR для своей работы хранит уменьшенние копии "прошлых кадров"
															// Эти кадры относительно похожи друг на друга, однако при включЄнном двойном рендере
															// в половине кадров оказывается картинка из второго рендера, и поскольку она часто может отличатся по цвету\яркости
															// то при попытке создания "плавного" перехода между ними получается эффект мерцания
	}
	//
	{
		t_LUM_src->surface_set		(rt_LUM_pool[gpu_id*2+0]->pSurface);
		t_LUM_dest->surface_set		(rt_LUM_pool[gpu_id*2+1]->pSurface);
	}

	RCache.set_CullMode(CULL_NONE);

	if (RImplementation.o.ssao_opt_data)
		phase_downsamp();
	else if (RImplementation.o.ssao_blur_on)
		phase_ssao();

	// low/hi RTs
	u_setrt				(rt_Generic_0, rt_Generic_1, nullptr, HW.pBaseZB);
	RCache.set_Stencil	(FALSE);

	bool split_the_scene_to_minimize_wait = ps_r_flags.test(R_FLAG_EXP_SPLIT_SCENE);

	// Draw skybox & clouds without Z-test to avoid silhouettes.
	// However, it's a bit slower process.
	RCache.set_ColorWriteEnable();
//	RCache.set_Z(FALSE);
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, FALSE));
	g_pGamePersistent->Environment().RenderSky();
	g_pGamePersistent->Environment().RenderClouds();
//	RCache.set_Z(TRUE);
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, TRUE));
	
	RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);	// stencil should be >= 1
	if (RImplementation.o.nvstencil)
	{
		u_stencil_optimize(FALSE);
		RCache.set_ColorWriteEnable();
	}

	// calc m-blur matrices
	Fmatrix m_previous, m_current;
	Fvector2 m_blur_scale;
	{
		static Fmatrix m_saved_viewproj;
		
		// (new-camera) -> (world) -> (old_viewproj)
		m_previous.mul		(m_saved_viewproj, RCache.xforms.m_invv);
		m_current.set		(Device.mProject);
		m_saved_viewproj.set(Device.mFullTransform);
		float scale			= ps_r_mblur / 2.0f;
		m_blur_scale.set	(scale, -scale).div(12.0f);
	}

	// Draw full-screen quad textured with our scene image
	if (!bMenuPP)
	{
		// Compute params
		CEnvDescriptorMixer& envdesc = *g_pGamePersistent->Environment().CurrentEnv;
		const float minamb = 0.001f;
		Fvector4 ambclr = { std::max(envdesc.ambient.x * 2,minamb),	std::max(envdesc.ambient.y * 2,minamb), std::max(envdesc.ambient.z * 2,minamb),	0.0f };
		ambclr.mul(ps_r_sun_lumscale_amb);

		Fvector4 envclr = { envdesc.hemi_color.x * 2 + EPS,	envdesc.hemi_color.y * 2 + EPS,	envdesc.hemi_color.z * 2 + EPS,	envdesc.weight };
		envclr.x *= 2*ps_r_sun_lumscale_hemi; 
		envclr.y *= 2*ps_r_sun_lumscale_hemi; 
		envclr.z *= 2*ps_r_sun_lumscale_hemi;
		Fvector4 sunclr, sundir;

		float fSSAONoise = 2.0f;
		fSSAONoise *= tan(deg2rad(67.5f/2.0f));
		fSSAONoise /= tan(deg2rad(Device.fFOV/2.0f));

		float fSSAOKernelSize = 150.0f;
		fSSAOKernelSize *= tan(deg2rad(67.5f/2.0f));
		fSSAOKernelSize /= tan(deg2rad(Device.fFOV/2.0f));

		// sun-params
		{
			light* pSun = (light*)RImplementation.Lights.sun._get();
			Fvector L_dir, L_clr;
			L_clr.set(pSun->color.r, pSun->color.g, pSun->color.b);
			float L_spec = u_diffuse2s(L_clr);
			Device.mView.transform_dir(L_dir, pSun->direction);
			L_dir.normalize();

			sunclr.set(L_clr.x, L_clr.y, L_clr.z, L_spec);
			sundir.set(L_dir.x, L_dir.y, L_dir.z, 0.0f);
		}

		// Fill VB
		float _w = float(Device.dwWidth);
		float _h = float(Device.dwHeight);
		p0.set(.5f / _w, .5f / _h);
		p1.set((_w + .5f) / _w, (_h + .5f) / _h);

		// Fill VB
		float scale_X = float(Device.dwWidth) / float(TEX_jitter);
		float scale_Y = float(Device.dwHeight) / float(TEX_jitter);

		FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine_VP->vb_stride, Offset);
		pv->set(hclip(EPS,		_w),	hclip(_h+EPS,	_h),	p0.x, p1.y, 0, 0,		scale_Y	);	pv++;
		pv->set(hclip(EPS,		_w),	hclip(EPS,		_h),	p0.x, p0.y, 0, 0,		0		);	pv++;
		pv->set(hclip(_w+EPS,	_w),	hclip(_h+EPS,	_h),	p1.x, p1.y, 0, scale_X,	scale_Y	);	pv++;
		pv->set(hclip(_w+EPS,	_w),	hclip(EPS,		_h),	p1.x, p0.y, 0, scale_X,	0		);	pv++;
		RCache.Vertex.Unlock(4, g_combine_VP->vb_stride);

		dxEnvDescriptorMixerRender &envdescren = *(dxEnvDescriptorMixerRender*)(&*envdesc.m_pDescriptorMixer);

		// Setup textures
		IDirect3DBaseTexture9* e0 = bMenuPP ? nullptr : envdescren.sky_r_textures_env[0].second->surface_get();
		IDirect3DBaseTexture9* e1 = bMenuPP ? nullptr : envdescren.sky_r_textures_env[1].second->surface_get();
		t_envmap_0->surface_set(e0); _RELEASE(e0);
		t_envmap_1->surface_set(e1); _RELEASE(e1);
	
		// Draw
		RCache.set_Element			(s_combine->E[0]	);
		RCache.set_Geometry			(g_combine_VP		);

		RCache.set_c				("L_ambient",		ambclr);

		RCache.set_c				("Ldynamic_color",	sunclr);
		RCache.set_c				("Ldynamic_dir",	sundir);

		RCache.set_c				("env_color",		envclr);
		RCache.set_c				("ssao_params",		fSSAONoise, fSSAOKernelSize, 0.0f, 0.0f);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// Forward rendering
	{
		u_setrt						(rt_Generic_0, nullptr, nullptr, HW.pBaseZB); // LDR RT
		RCache.set_CullMode			(CULL_CCW);
		RCache.set_Stencil			(FALSE);
		RCache.set_ColorWriteEnable	();
		if (bMenuPP)
			g_pGamePersistent->OnRenderPPUI_main();	// PP-UI
		else
			RImplementation.render_forward();
	}

	//	Igor: for volumetric lights
	//	combine light volume here
	if (m_bHasActiveVolumetric)
		phase_combine_volumetric();

	// Perform blooming filter and distortion if needed
	RCache.set_Stencil	(FALSE);
	phase_bloom			(); // HDR RT invalidated here

	// Distortion filter
	bool bDistort = (RImplementation.o.distortion_enabled && (!RImplementation.mapDistort.empty() || bMenuPP));
	if (bDistort)
	{
		RCache.set_CullMode			(CULL_CCW);
		RCache.set_Stencil			(FALSE);
		RCache.set_ColorWriteEnable	();

		// rt_Generic_1: xy - displacement, z - bloom factor, w - opacity
		u_setrt						(rt_Generic_1, nullptr, nullptr, HW.pBaseZB);
		RCache.Clear				(0L, nullptr, D3DCLEAR_TARGET, color_rgba(127, 127, 0, 0), 1.0f, 0L);

		if (bMenuPP)
			g_pGamePersistent->OnRenderPPUI_PP(); // PP-UI
		else
			RImplementation.r_dsgraph_render_distort();
	}
	
	// PP enabled ?
	// Render to RT texture to be able to copy RT even in windowed mode.
	bool bComplexPP = u_need_PP() || (ps_r_pp_aa_mode > 0) || ps_r_flags.test(R_FLAG_RAIN_DROPS);
	if (bMenuPP)
		bComplexPP = false;
	else
	{

		// Screen space sunshafts
		if (need_to_render_sunshafts())
		{
			PIX_EVENT(phase_ss_ss);
			PhaseSSSS();
		}

		if (Puddles->m_bLoaded)
			phase_puddles();
	}

	/////////////////////////////
	// Combine 2:
	// 1. Distortion
	// 2. DOF
	// 3. Motion blur
	// 4. Bloom

	// If complex PP required - output to temporary RT
	// otherwise render directly to back buffer.
	// Cases when complex PP used:
	// - PPE
	// - Anti-aliasing (FXAA, SMAA)
	// - On-screen rain drops
	if (bComplexPP)
		u_setrt(rt_Color, nullptr, nullptr, HW.pBaseZB); // LDR RT
	else
		u_setrt(Device.dwWidth, Device.dwHeight, HW.pBaseRT, nullptr, nullptr, HW.pBaseZB);

	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(FALSE);

	// 
	float _w = float(Device.dwWidth);
	float _h = float(Device.dwHeight);
	float ddw = 1.0f/_w;
	float ddh = 1.0f/_h;
	p0.set(0.5f / _w, 0.5f / _h);
	p1.set((_w + 0.5f) / _w, (_h + 0.5f) / _h);

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(EPS,    _h+EPS, EPS, 1.0f, 0.0f, p0.x, p1.y); pv++;
	pv->set(EPS,    EPS,    EPS, 1.0f, 0.0f, p0.x, p0.y); pv++;
	pv->set(_w+EPS, _h+EPS, EPS, 1.0f, 0.0f, p1.x, p1.y); pv++;
	pv->set(_w+EPS, EPS,    EPS, 1.0f, 0.0f, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	//	Set up variable
	Fvector2 vDofKernel;
	vDofKernel.set(0.5f / Device.dwWidth, 0.5f / Device.dwHeight);
	vDofKernel.mul(ps_r_dof_kernel_size);

	// Draw COLOR
	RCache.set_Element	(s_combine->E[bDistort ? 2 : 1]); // look at blender_combine.cpp
	RCache.set_c		("m_current", m_current);
	RCache.set_c		("m_previous", m_previous);
	RCache.set_c		("m_blur", m_blur_scale.x, m_blur_scale.y, 0.0f, 0.0f);
	Fvector3 dof;
	g_pGamePersistent->GetCurrentDof(dof);
	RCache.set_c		("dof_params", dof.x, dof.y, dof.z, ps_r_dof_sky);
	RCache.set_c		("dof_kernel", vDofKernel.x, vDofKernel.y, ps_r_dof_kernel_size, 0.0f);
	
	RCache.set_Geometry	(g_combine);
	RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	RCache.set_Stencil	(FALSE);

	// if FP16-BLEND !not! supported - draw flares here, overwise they are already in the bloom target
	g_pGamePersistent->Environment().RenderFlares();	// lens-flares

	// PP-if required
	if (bComplexPP)
	{
		// Postprocess anti-aliasing
		if (ps_r_pp_aa_mode > 0)
		{
			PIX_EVENT(phase_pp_aa);
			PhaseAA();
		}

		// Rain droplets on screen
		if (ps_r_flags.test(R_FLAG_RAIN_DROPS))
		{
			PIX_EVENT(phase_rain_droplets);
			PhaseRainDrops();
		}

		PIX_EVENT(phase_pp);
		phase_pp();
	}

	// Re-adapt luminance
	RCache.set_Stencil	(FALSE);

	//*** exposure-pipeline-clear
	{
		std::swap					(rt_LUM_pool[gpu_id*2+0],rt_LUM_pool[gpu_id*2+1]);
		t_LUM_src->surface_set		(nullptr);
		t_LUM_dest->surface_set		(nullptr);
	}

#ifdef DEBUG
	RCache.set_CullMode	( CULL_CCW );
	static	xr_vector<Fplane>		saved_dbg_planes;
	if (bDebug)		
		saved_dbg_planes= dbg_planes;
	else			
		dbg_planes		= saved_dbg_planes;

	for (u32 it=0; it<dbg_planes.size(); it++)
	{
		Fplane&		P	=	dbg_planes[it];
		Fvector		zero	;
		zero.mul	(P.n,P.d);
		
		Fvector             L_dir,L_up=P.n,L_right;
		L_dir.set           (0,0,1);                if (_abs(L_up.dotproduct(L_dir))>.99f)  L_dir.set(1,0,0);
		L_right.crossproduct(L_up,L_dir);           L_right.normalize       ();
		L_dir.crossproduct  (L_right,L_up);         L_dir.normalize         ();

		Fvector				p0,p1,p2,p3;
		float				sz	= 100.f;
		p0.mad				(zero,L_right,sz).mad	(L_dir,sz);
		p1.mad				(zero,L_right,sz).mad	(L_dir,-sz);
		p2.mad				(zero,L_right,-sz).mad	(L_dir,-sz);
		p3.mad				(zero,L_right,-sz).mad	(L_dir,+sz);
		RCache.dbg_DrawTRI	(Fidentity,p0,p1,p2,0xffffffff);
		RCache.dbg_DrawTRI	(Fidentity,p2,p3,p0,0xffffffff);
	}

	static	xr_vector<dbg_line_t>	saved_dbg_lines;
	if (bDebug)
		saved_dbg_lines	= dbg_lines;
	else
		dbg_lines		= saved_dbg_lines;

	HW.pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	RCache.set_ZFunc(D3DCMP_LESSEQUAL);
	HW.pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	for (u32 it=0; it<dbg_lines.size(); it++)
	{
		RCache.dbg_DrawLINE		(Fidentity,dbg_lines[it].P0,dbg_lines[it].P1,dbg_lines[it].color);
	}

	dbg_spheres.clear	();
	dbg_lines.clear		();
	dbg_planes.clear	();
#endif
}

void CRenderTarget::phase_wallmarks ()
{
	// Targets
	RCache.set_RT(nullptr, 2);
	RCache.set_RT(nullptr, 1);
	u_setrt(rt_Color, nullptr, nullptr, HW.pBaseZB);

	// Stencil	- draw only where stencil >= 0x1
	RCache.set_Stencil			(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
	RCache.set_CullMode			(CULL_CCW);
	RCache.set_ColorWriteEnable	(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}

void CRenderTarget::phase_combine_volumetric()
{
	u32 Offset = 0;
	Fvector2 p0,p1;

	u_setrt(rt_Generic_0, rt_Generic_1, nullptr, HW.pBaseZB);

	//	Sets limits to both render targets
	RCache.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
	{
		// Compute params
		CEnvDescriptorMixer& envdesc= *g_pGamePersistent->Environment().CurrentEnv;
		const float minamb = 0.001f;
		Fvector4 ambclr = { std::max(envdesc.ambient.x * 2,minamb), std::max(envdesc.ambient.y * 2,minamb), std::max(envdesc.ambient.z * 2,minamb),	0.0f };
		ambclr.mul(ps_r_sun_lumscale_amb);

		Fvector4 envclr = { envdesc.hemi_color.x * 2 + EPS,	envdesc.hemi_color.y * 2 + EPS,	envdesc.hemi_color.z * 2 + EPS,	envdesc.weight };
		envclr.x *= 2 * ps_r_sun_lumscale_hemi;
		envclr.y *= 2 * ps_r_sun_lumscale_hemi;
		envclr.z *= 2 * ps_r_sun_lumscale_hemi;
		Fvector4 sunclr, sundir;

		// sun-params
		{
			light* pSun = (light*)RImplementation.Lights.sun._get();
			Fvector L_dir,L_clr;
			float L_spec;
			L_clr.set					(pSun->color.r, pSun->color.g, pSun->color.b);
			L_spec						= u_diffuse2s	(L_clr);
			Device.mView.transform_dir	(L_dir, pSun->direction);
			L_dir.normalize				();

			sunclr.set				(L_clr.x,L_clr.y,L_clr.z,L_spec);
			sundir.set				(L_dir.x,L_dir.y,L_dir.z,0);
		}

		// Fill VB
		float	_w					= float(Device.dwWidth);
		float	_h					= float(Device.dwHeight);
		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Fill VB
		float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
		float	scale_Y				= float(Device.dwHeight)/ float(TEX_jitter);

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine_VP->vb_stride,Offset);
		pv->set						(hclip(EPS,		_w),	hclip(_h+EPS,	_h),	p0.x, p1.y, 0, 0,			scale_Y	);	pv++;
		pv->set						(hclip(EPS,		_w),	hclip(EPS,		_h),	p0.x, p0.y, 0, 0,			0		);	pv++;
		pv->set						(hclip(_w+EPS,	_w),	hclip(_h+EPS,	_h),	p1.x, p1.y, 0, scale_X,	scale_Y	);	pv++;
		pv->set						(hclip(_w+EPS,	_w),	hclip(EPS,		_h),	p1.x, p0.y, 0, scale_X,	0		);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_VP->vb_stride);

		// Draw
		RCache.set_Element			(s_combine_volumetric->E[0]	);
		RCache.set_Geometry			(g_combine_VP		);

		RCache.set_c				("L_ambient",		ambclr	);

		RCache.set_c				("Ldynamic_color",	sunclr	);
		RCache.set_c				("Ldynamic_dir",	sundir	);

		RCache.set_c				("env_color",		envclr	);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
	RCache.set_ColorWriteEnable();
}
