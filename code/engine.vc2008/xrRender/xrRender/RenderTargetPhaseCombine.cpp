#include "stdafx.h"
#include "../../xrEngine/IGame_Persistent.h"
#include "../../xrEngine/Environment.h"
#include "../../xrEngine/DirectXMathExternal.h"

#include "r2_puddles.h"
#include "../xrRender/dxEnvironmentRender.h"
#include "../xrRender/dxRenderDeviceRender.h"
#include "../../xrEngine/Spectre/Spectre.h"

void CRenderTarget::phase_combine()
{
	Device.Statistic->Render_Combine_Begin.Begin();
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
		t_LUM_src->surface_set	(rt_LUM_pool[gpu_id*2+0]->pSurface);
		t_LUM_dest->surface_set	(rt_LUM_pool[gpu_id*2+1]->pSurface);
	}

	RCache.set_CullMode(CULL_NONE);

	// SSAO
	if (RImplementation.o.ssao_opt_data)
		phase_downsamp();
	else if (RImplementation.o.ssao_blur_on)
		phase_ssao();

	// low/hi RTs
	if (!RImplementation.o.dx10_msaa)
	{
		u_setrt(rt_Generic_0, rt_Generic_1, nullptr, HW.pBaseZB);
		RCache.Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	}
	else
	{
		u_setrt(rt_Generic_0_r, rt_Generic_1_r, nullptr, RImplementation.Target->rt_MSAADepth->pZRT);
		RCache.Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	}

	RCache.set_Stencil	(FALSE);
	Device.Statistic->Render_Combine_Begin.End();

	// Draw skybox & clouds without Z-test to avoid silhouettes.
	// However, it's a bit slower process.
	RCache.set_ColorWriteEnable();
	Device.Statistic->Render_Combine_Sky.Begin();
	Environment().RenderSky();
	Device.Statistic->Render_Combine_Sky.End();
	Device.Statistic->Render_Combine_Cloud.Begin();
	Environment().RenderClouds();
	Device.Statistic->Render_Combine_Cloud.End();
	
	RCache.set_Stencil(TRUE, D3D11_COMPARISON_LESS_EQUAL, 0x01, 0xff, 0x00);	// stencil should be >= 1

	// calc m-blur matrices
	Device.Statistic->Render_Combine_Combine1.Begin();
	Fmatrix m_previous, m_current;
	Fvector2 m_blur_scale;
	{
		static Fmatrix m_saved_viewproj;
		
		// (new-camera) -> (world) -> (old_viewproj)
		m_previous.mul		( m_saved_viewproj, RCache.xforms.m_invv );
		m_current.set		(Device.mProject);
		m_saved_viewproj.set(Device.mFullTransform);
		float scale			= ps_r_mblur / 2.0f;
		m_blur_scale.set	(scale, -scale).div(12.0f);
	}

	// Combine 1: draw full-screen quad textured with our scene image
	{
		PIX_EVENT(combine_1);

		u32 Offset = 0;

		float scale_X	= float(Device.dwWidth) / float(TEX_jitter);
		float scale_Y	= float(Device.dwHeight) / float(TEX_jitter);

		// Compute params
		CEnvDescriptorMixer& envdesc = *Environment().CurrentEnv;
		const float minamb = 0.001f;
		Fvector4 ambclr = { std::max(envdesc.ambient.x * 2,minamb),	std::max(envdesc.ambient.y * 2,minamb), std::max(envdesc.ambient.z * 2,minamb),	0.0f };
		ambclr.mul(ps_r_sun_lumscale_amb);

		Fvector4 envclr = { envdesc.hemi_color.x * 2 + EPS,	envdesc.hemi_color.y * 2 + EPS,	envdesc.hemi_color.z * 2 + EPS,	envdesc.weight };
		envclr.x *= 2*ps_r_sun_lumscale_hemi; 
		envclr.y *= 2*ps_r_sun_lumscale_hemi; 
		envclr.z *= 2*ps_r_sun_lumscale_hemi;
		Fvector4 sunclr, sundir;

		// Sun params
		{
			light* pSun = (light*)RImplementation.Lights.sun._get();
			Fvector L_dir, L_clr;
			L_clr.set(pSun->color.r, pSun->color.g, pSun->color.b);
			float L_spec = Diffuse::u_diffuse2s(L_clr);
			Device.mView.transform_dir(L_dir, pSun->direction);
			L_dir.normalize();

			sunclr.set(L_clr.x, L_clr.y, L_clr.z, L_spec);
			sundir.set(L_dir.x, L_dir.y, L_dir.z, 0.0f);
		}

		dxEnvDescriptorMixerRender &envdescren = *(dxEnvDescriptorMixerRender*)(&*envdesc.m_pDescriptorMixer);

		// Setup textures
		ID3DBaseTexture* e0 = envdescren.sky_r_textures_env[0].second->surface_get();
		ID3DBaseTexture* e1 = envdescren.sky_r_textures_env[1].second->surface_get();
		t_envmap_0->surface_set(e0); _RELEASE(e0);
		t_envmap_1->surface_set(e1); _RELEASE(e1);

		// Fill VB
		FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
		pv->set(-1,  1,	0, 1, 0, 0,			scale_Y	); pv++;
		pv->set(-1, -1,	0, 0, 0, 0,			0		); pv++;
		pv->set( 1,  1,	1, 1, 0, scale_X,	scale_Y	); pv++;
		pv->set( 1, -1,	1, 0, 0, scale_X,	0		); pv++;
		RCache.Vertex.Unlock(4, g_combine->vb_stride);

		RCache.set_Geometry(g_combine);
	
		// Set variables
		RCache.set_Element			(s_combine->E[0]);

		RCache.set_c				("L_ambient",		ambclr);
		RCache.set_c				("Ldynamic_color",	sunclr);
		RCache.set_c				("Ldynamic_dir",	sundir);
		RCache.set_c				("env_color",		envclr);

		if (!RImplementation.o.dx10_msaa)
			RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
		else
		{
			RCache.set_Stencil	(TRUE, D3D11_COMPARISON_EQUAL, 0x01, 0x81, 0);
			RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
			if (RImplementation.o.dx10_msaa_opt)
			{
				RCache.set_Element	(s_combine_msaa[0]->E[0]);
				RCache.set_Stencil	(TRUE, D3D11_COMPARISON_EQUAL, 0x81, 0x81, 0);
				RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
			}
			else
			{
				for (u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i)
				{
					RCache.set_Element			(s_combine_msaa[i]->E[0]);
					StateManager.SetSampleMask	(u32(1) << i);
					RCache.set_Stencil			(TRUE, D3D11_COMPARISON_EQUAL, 0x81, 0x81, 0);
					RCache.Render				(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
				}
				StateManager.SetSampleMask(0xffffffff);
			}
			RCache.set_Stencil(FALSE, D3D11_COMPARISON_EQUAL, 0x01, 0xff, 0);
		}
	}

	Device.Statistic->Render_Combine_Combine1.End();

	// Forward rendering
	{
		ScopeStatTimer forwardRenderingTimer(Device.Statistic->Render_Combine_ForwardRendering);
		PIX_EVENT(Forward_rendering);
		// LDR RT
		if (!RImplementation.o.dx10_msaa)
			u_setrt(rt_Generic_0, nullptr, nullptr, HW.pBaseZB);
		else
			u_setrt(rt_Generic_0_r, nullptr, nullptr, RImplementation.Target->rt_MSAADepth->pZRT);

		RCache.set_CullMode			(CULL_CCW);
		RCache.set_Stencil			(FALSE);
		RCache.set_ColorWriteEnable	();

		RImplementation.render_forward();
	}


	// Combine light volume here
	if (m_bHasActiveVolumetric)
	{
		PIX_EVENT(phase_combine_volumetric);
		phase_combine_volumetric();
	}

	if (RImplementation.o.dx10_msaa)
	{
		// We need to resolve rt_Generic_1 into rt_Generic_1_r
		RCache.ResolveSubresource(rt_Generic_1->pTexture->surface_get(), 0, rt_Generic_1_r->pTexture->surface_get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
		RCache.ResolveSubresource(rt_Generic_0->pTexture->surface_get(), 0, rt_Generic_0_r->pTexture->surface_get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	// Blooming filter
	RCache.set_Stencil(FALSE);
	phase_bloom(); // HDR RT invalidated here

	// Distortion filter
	bool bDistort = (RImplementation.o.distortion_enabled && !RImplementation.mapDistort.empty());
	if (bDistort)
	{
		RCache.set_CullMode			(CULL_CCW);
		RCache.set_Stencil			(FALSE);
		RCache.set_ColorWriteEnable	();

		// rt_Generic_1: xy - displacement, z - bloom factor, w - opacity
		if (!RImplementation.o.dx10_msaa)
			u_setrt(rt_Generic_1, nullptr, nullptr, HW.pBaseZB);
		else
			u_setrt(rt_Generic_1_r, nullptr, nullptr, RImplementation.Target->rt_MSAADepth->pZRT);

		RCache.Clear(0L, nullptr, D3DCLEAR_TARGET, color_rgba(127, 127, 0, 0), 1.0f, 0L);
		RImplementation.r_dsgraph_render_distort();
	}
	
	// Screen space reflections
//	if (ps_r_reflections_mode == SCREEN_SPACE)
//		PhaseReflectionsSS();

	// Screen space sunshafts
	if (need_to_render_sunshafts())
	{
		PIX_EVENT(phase_ss_ss);
		PhaseSSSS();
	}


	if (Puddles && Puddles->m_bLoaded)
	{
		PIX_EVENT(phase_puddles);
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
	// - Vignette effect
	{
		PIX_EVENT(combine_2);

		u32 Offset = 0;
		Fvector2 p0, p1;

		float _w = float(Device.dwWidth);
		float _h = float(Device.dwHeight);

		p0.set(0.0f, 0.0f);
		p1.set(1.0f, 1.0f);

		// LDR RT
		if (RImplementation.o.dx10_msaa)
			u_setrt(rt_Generic, nullptr, nullptr, HW.pBaseZB);
		else
			u_setrt(rt_Color, nullptr, nullptr, HW.pBaseZB);

		RCache.set_CullMode	(CULL_NONE);
		RCache.set_Stencil	(FALSE);

		// Fill VB
		FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
		pv->set(EPS,    _h+EPS, EPS, 1.0f, 0, p0.x, p1.y); pv++;
		pv->set(EPS,    EPS,    EPS, 1.0f, 0, p0.x, p0.y); pv++;
		pv->set(_w+EPS, _h+EPS, EPS, 1.0f, 0, p1.x, p1.y); pv++;
		pv->set(_w+EPS, EPS,    EPS, 1.0f, 0, p1.x, p0.y); pv++;
		RCache.Vertex.Unlock(4, g_combine->vb_stride);

		// Set up variables
		Fvector2 vDofKernel;
		vDofKernel.set(0.5f / Device.dwWidth, 0.5f / Device.dwHeight);
		vDofKernel.mul(ps_r_dof_kernel_size);

		if (!RImplementation.o.dx10_msaa)
			RCache.set_Element(s_combine->E[bDistort ? 2 : 1]);
		else
			RCache.set_Element(s_combine_msaa[0]->E[bDistort ? 2 : 1]);	

		RCache.set_c		("m_current", m_current);
		RCache.set_c		("m_previous", m_previous);
		RCache.set_c		("m_blur", m_blur_scale.x, m_blur_scale.y, 0.0f, 0.0f);
		Fvector3 dof;
		g_pGamePersistent->GetCurrentDof(dof);
		RCache.set_c		("dof_params", dof.x, dof.y, dof.z, ps_r_dof_sky);
		RCache.set_c		("dof_kernel", vDofKernel.x, vDofKernel.y, ps_r_dof_kernel_size, 0.0f);

		// Draw COLOR
		RCache.set_Geometry	(g_combine);
		RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	}

	RCache.set_Stencil	(FALSE);

	// if FP16-BLEND !not! supported - draw flares here, overwise they are already in the bloom target
	Environment().RenderFlares();	// lens-flares

	// Postprocess anti-aliasing
	if (ps_r_pp_aa_quality > 0)
	{
		PIX_EVENT(phase_pp_aa);
		PhaseAA();
	}

	// Rain droplets on screen
	if (ps_r_postscreen_flags.test(R_FLAG_RAIN_DROPS))
	{
		PIX_EVENT(phase_rain_droplets);
		PhaseRainDrops();
	}

	if (ps_r_postscreen_flags.test(R_FLAG_CHROMATIC))
	{
		PIX_EVENT(phase_chromatic);
		PhaseChromaticAmb();
	}

	if (ps_r_postscreen_flags.test(R_FLAG_GRADING))
	{
		PIX_EVENT(phase_grading);
		PhaseGrading();
	}

	// Vignette effect
	if (ps_r_postscreen_flags.test(R_FLAG_VIGNETTE))
	{
		PIX_EVENT(phase_vignette);
		PhaseVignette();
	}

	// PPE
	if (u_need_PP())
	{
		PIX_EVENT(phase_pp);
		phase_pp();
	}

	SpectreCallback::SecondaryCombine->Invoke(SpectreObjectId);

	// Final stage: copy rt_Color to back buffer. Let's do it with GPU.
	{
		if (RImplementation.o.dx10_msaa)
			RenderScreenQuad(Device.dwWidth, Device.dwHeight, HW.pBaseRT, s_combine_msaa[0]->E[3]);
		else
			RenderScreenQuad(Device.dwWidth, Device.dwHeight, HW.pBaseRT, s_combine->E[3]);
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

#ifdef USE_DX11
	StateManager.SetDepthEnable(TRUE);
	StateManager.SetDepthFunc(D3D11_COMPARISON_LESS_EQUAL);
	StateManager.Apply();
#else
	HW.pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	RCache.set_ZFunc(D3D11_COMPARISON_LESS_EQUAL);
	HW.pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
#endif
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
#ifdef USE_DX11
	if (!RImplementation.o.dx10_msaa)
		u_setrt(rt_Color, nullptr, nullptr, HW.pBaseZB);
	else
		u_setrt(rt_Color, nullptr, nullptr, rt_MSAADepth->pZRT);
#else
	u_setrt(rt_Color, nullptr, nullptr, HW.pBaseZB);
#endif

	// Stencil	- draw only where stencil >= 0x1
	RCache.set_Stencil			(TRUE, D3D11_COMPARISON_LESS_EQUAL, 0x01, 0xff, 0x00);
	RCache.set_CullMode			(CULL_CCW);
	RCache.set_ColorWriteEnable	(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}

void CRenderTarget::phase_combine_volumetric()
{
	u32 Offset = 0;

	float scale_X	= float(Device.dwWidth) / float(TEX_jitter);
	float scale_Y	= float(Device.dwHeight) / float(TEX_jitter);

#ifdef USE_DX11
	if (!RImplementation.o.dx10_msaa)
		u_setrt(rt_Generic_0, rt_Generic_1, nullptr, HW.pBaseZB);
	else
		u_setrt(rt_Generic_0_r, rt_Generic_1_r, nullptr, RImplementation.Target->rt_MSAADepth->pZRT);
#else
	u_setrt(rt_Generic_0, rt_Generic_1, nullptr, HW.pBaseZB);
#endif

	// Sets limits to both render targets
	RCache.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
	{
		// Fill VB
		FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
		pv->set(-1,	 1,	0, 1, 0, 0,			scale_Y	); pv++;
		pv->set(-1,	-1,	0, 0, 0, 0,			0		); pv++;
		pv->set( 1,  1,	1, 1, 0, scale_X,	scale_Y	); pv++;
		pv->set( 1, -1,	1, 0, 0, scale_X,	0		); pv++;
		RCache.Vertex.Unlock(4, g_combine->vb_stride);

		RCache.set_Geometry(g_combine);
		// Draw
		RCache.set_Element(s_combine_volumetric->E[0]);

		RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	}
	RCache.set_ColorWriteEnable();
}
