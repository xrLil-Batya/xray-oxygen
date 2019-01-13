#include "stdafx.h"
#include "../../xrEngine/IGame_Persistent.h"
#include "../../xrEngine/CustomHUD.h"
#include "../../xrEngine/xr_object.h"

#include "FBasicVisual.h"
#include "QueryHelper.h"
#include "dxGlowManager.h"

IC bool pred_sp_sort(ISpatial* _1, ISpatial* _2)
{
	float d1 = _1->spatial.sphere.P.distance_to_sqr(Device.vCameraPosition);
	float d2 = _2->spatial.sphere.P.distance_to_sqr(Device.vCameraPosition);
	return	d1 < d2;
}

//MatthewKush to all: I was completely wrong before
//I shouldn't smoke so much cheeba
//Current goal: deffered lighting > deffered shading > forward+ shading
// Giperion to MatthewKush: smoking is bad for you health and our engine >_< .
void CRender::render_main(Fmatrix& mCombined)
{
	PIX_EVENT(render_main);
	marker++;

	// Calculate sector(s) and their objects
	if (pLastSector)
	{
		//!!!
		//!!! BECAUSE OF PARALLEL HOM RENDERING TRY TO DELAY ACCESS TO HOM AS MUCH AS POSSIBLE
		//!!!
		{
			// Traverse object database
			g_SpatialSpace->q_frustum(
				lstRenderables,
				ISpatial_DB::O_ORDERED,
				STYPE_RENDERABLE + STYPE_LIGHTSOURCE,
				ViewBase);

			// (almost) Exact sorting order (front-to-back)
			std::sort(lstRenderables.begin(), lstRenderables.end(), pred_sp_sort);

			// Determine visibility for dynamic part of scene
			set_Object(nullptr);
			u32 uID_LTRACK = 0xffffffff;
			if (phase == PHASE_NORMAL)
			{
				uLastLTRACK++;
				if (!lstRenderables.empty())
					uID_LTRACK = uLastLTRACK % lstRenderables.size();

				// Update light-vis for current entity / actor
				CObject* pCurrViewEntity = g_pGameLevel->CurrentViewEntity();
				if (pCurrViewEntity)
				{
					CROS_impl* R = (CROS_impl*)pCurrViewEntity->ROS();
					if (R)
						R->update(pCurrViewEntity);
				}

				// Update light-vis for selected entity
				// Track lighting environment
				if (!lstRenderables.empty())
				{
					IRenderable* pRenderable = lstRenderables[uID_LTRACK]->dcast_Renderable();
					if (pRenderable)
					{
						CROS_impl* T = (CROS_impl*)pRenderable->renderable_ROS();
						if (T)
							T->update(pRenderable);
					}
				}
			}
		}

		// Traverse sector/portal structure
		PortalTraverser.traverse	
			(
			pLastSector,
			ViewBase,
			Device.vCameraPosition,
			mCombined,
			CPortalTraverser::VQ_HOM + CPortalTraverser::VQ_SSA + CPortalTraverser::VQ_FADE
			//. disabled scissoring (HW.Caps.bScissor?CPortalTraverser::VQ_SCISSOR:0) // Generate scissoring info
			);

		// Determine visibility for static geometry hierrarhy
		for (IRender_Sector* r_sector : PortalTraverser.r_sectors)
		{
			CSector* pSector = (CSector*)r_sector;
			dxRender_Visual* pSectorRoot = pSector->root();
			for (CFrustum& r_frustum : pSector->r_frustums)
			{
				set_Frustum(&r_frustum);
				add_Geometry(pSectorRoot);
			}
		}

		// Traverse frustums
		for (ISpatial* pSpatial : lstRenderables)
		{
			pSpatial->spatial_updatesector();
			CSector* pSector = (CSector*)pSpatial->spatial.sector;
			if	(!pSector)
				continue; // Disassociated from S/P structure

			if (pSpatial->spatial.type & STYPE_LIGHTSOURCE)
			{
				// Lightsource
				light* pLight = (light*)(pSpatial->dcast_Light());
				VERIFY(pLight);
				float lod = pLight->get_LOD();
				if (lod > EPS_L)
				{
					vis_data& vis = pLight->get_homdata();
					if (HOM.visible(vis))
						Lights.add_light(pLight);
				}
				continue;
			}

			if (PortalTraverser.i_marker != pSector->r_marker)
				continue; // Inactive (untouched) sector

			for (CFrustum& view : pSector->r_frustums)
			{
				if (!view.testSphere_dirty(pSpatial->spatial.sphere.P, pSpatial->spatial.sphere.R))
					continue;

				if (pSpatial->spatial.type & STYPE_RENDERABLE)
				{
					IRenderable* renderable	= pSpatial->dcast_Renderable();
					bool bSphere = view.testSphere_dirty(pSpatial->spatial.sphere.P, pSpatial->spatial.sphere.R);

					if (!renderable)
					{
						if (ps_r_flags.is(R_FLAG_GLOW_USE))
							continue;

						CGlow* pGlow = dynamic_cast<CGlow*>(pSpatial);
						VERIFY2(pGlow, "Glow don't created!");

						if (pGlow)
						{
							if (bSphere)
								Glows->add(pGlow);
							else
								pGlow->hide_glow();
						}
					}
					else if (bSphere)
					{
						// Occlusion
						// Casting is faster then using getVis method
						vis_data& v_orig	= ((dxRender_Visual*)renderable->renderable.visual)->vis;
						vis_data v_copy		= v_orig;
						v_copy.box.xform(renderable->renderable.xform);
						bool bVisible = HOM.visible(v_copy);
						v_orig.marker		= v_copy.marker;
						v_orig.accept_frame	= v_copy.accept_frame;
						v_orig.hom_frame	= v_copy.hom_frame;
						v_orig.hom_tested	= v_copy.hom_tested;

						// Exit loop on frustums
						if (!bVisible)
							break;	

						// Rendering
						set_Object(renderable);
						renderable->renderable_Render();
						set_Object(nullptr);
					}
				}
				break; // Exit loop on frustums
			}
		}
		if (g_pGameLevel && (phase == PHASE_NORMAL))
			g_hud->Render_Last(); // HUD
	}
	else
	{
		set_Object(nullptr);
		if (g_pGameLevel && (phase == PHASE_NORMAL))
			g_hud->Render_Last(); // HUD
	}
}

void CRender::render_menu()
{
	PIX_EVENT(render_menu);
	// Globals
	RCache.set_CullMode(CULL_CCW);
	RCache.set_Stencil(FALSE);
	RCache.set_ColorWriteEnable();

	// Main Render
	{
		Target->u_setrt(Target->rt_Generic_0, nullptr, nullptr, HW.pBaseZB); // LDR RT
		g_pGamePersistent->OnRenderPPUI_main();
	}

	// Distort
	{
		Target->u_setrt(Target->rt_Generic_1, nullptr, nullptr, HW.pBaseZB); // Now RT is a distortion mask
		RCache.Clear(0, nullptr, D3DCLEAR_TARGET, color_rgba(127, 127, 0, 127), 1.0f, 0);
		g_pGamePersistent->OnRenderPPUI_PP();
	}

	// Actual Display
	Target->u_setrt		(Device.dwWidth, Device.dwHeight, HW.pBaseRT, nullptr, nullptr, HW.pBaseZB);
	RCache.set_Shader	(Target->s_menu);
	RCache.set_Geometry	(Target->g_menu);

	u32 Offset;
	u32 C		= color_rgba(255, 255, 255, 255);
	float _w	= float(Device.dwWidth);
	float _h	= float(Device.dwHeight);
	float d_Z	= EPS_S;
	float d_W	= 1.0f;
	Fvector2 p0, p1;
	p0.set(0.5f / _w, 0.5f / _h);
	p1.set((_w + 0.5f) / _w, (_h + 0.5f) / _h);

	FVF::TL* pv = (FVF::TL*) RCache.Vertex.Lock(4, Target->g_menu->vb_stride, Offset);
	pv->set(EPS,		_h + EPS,	d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(EPS,		EPS,		d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(_w + EPS,	_h + EPS,	d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(_w + EPS,	EPS,		d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, Target->g_menu->vb_stride);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRender::Render()
{
	ScopeStatTimer scopeTimer(Device.Statistic->Render_CRenderRender);
	PIX_EVENT(CRender_Render);

	VERIFY(mapDistort.empty());

	rmNormal();

	bool bMenuPP = g_pGamePersistent ? g_pGamePersistent->OnRenderPPUI_query() : false;
	if (bMenuPP)
	{
		ScopeStatTimer mainMenuScopeTimer(Device.Statistic->Render_CRenderRender_MainMenu);
		render_menu();
		return;
	};

	IMainMenu* pMainMenu = g_pGamePersistent ? g_pGamePersistent->m_pMainMenu : nullptr;
	bool bMenu = pMainMenu ? pMainMenu->CanSkipSceneRendering() : false;

	if (!(g_pGameLevel && g_hud) || bMenu)
		return;

	if (m_bFirstFrameAfterReset)
	{
		m_bFirstFrameAfterReset = false;
		return;
	}

	Device.Statistic->Render_CRenderRender_ScenePrepare.Begin();
	// Configure
	RImplementation.o.distortion = FALSE;		// disable distorion
	Fcolor sun_color = ((light*)Lights.sun._get())->color;
	bool bSUN = ps_r_flags.test(R_FLAG_SUN) && (Diffuse::u_diffuse2s(sun_color.r, sun_color.g, sun_color.b) > EPS);

	// HOM
	ViewBase.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);
	View = nullptr;

	Target->phase_scene_prepare();
    //RCache.set_ZB( RImplementation.Target->rt_Depth->pZRT ); //NOT EVEN a depth prepass :P

	Device.Statistic->Render_CRenderRender_ScenePrepare.End();
	//*******
	// Sync point
	Device.Statistic->Render_CRenderRender_WaitForFrame.Begin();
		
	CTimer T; T.Start();
	BOOL result = FALSE;
	HRESULT	hr = S_FALSE;
	while ((hr = GetData(q_sync_point[q_sync_count], &result, sizeof(result))) == S_FALSE)
	{
		if (!SwitchToThread())
			Sleep(ps_r_wait_sleep);

		if (T.GetElapsed_ms() > 500)
		{
			result = FALSE;
			break;
		}
	}
	
	q_sync_count = (q_sync_count+1)%HW.Caps.iGPUNum;
	CHK_DX(EndQuery(q_sync_point[q_sync_count]));
	Device.Statistic->Render_CRenderRender_WaitForFrame.End();

	//******* Main calc - DEFERRED RENDERER
	Device.Statistic->Render_CRenderRender_render_main.Begin();
	r_pmask(true, false, true);	// Enable priority "0",+ capture wmarks
	set_Recorder(bSUN ? &main_coarse_structure : nullptr);
	phase = PHASE_NORMAL;
	render_main(CastToGSCMatrix(Device.mFullTransform));
	set_Recorder(nullptr);
	r_pmask(true, false); // Disable priority "1"
	Device.Statistic->Render_CRenderRender_render_main.End();

	//******* Main render :: PART-0	-- first
	{
		ScopeStatTimer deferPart0Timer(Device.Statistic->Render_CRenderRender_DeferPart0);
		PIX_EVENT(DEFER_PART0);

		Target->phase_scene_begin();
		r_dsgraph_render_hud();
		r_dsgraph_render_graph(0);
		r_dsgraph_render_lods(true, true);
		if (Details)
			Details->Render();
		Target->phase_scene_end();
	} 

	//******* Occlusion testing of volume-limited light-sources
	Device.Statistic->Render_CRenderRender_LightVisibility.Begin();
	Target->phase_occq();
	LP_normal.clear();
	LP_pending.clear();

#ifdef USE_DX11
	if (RImplementation.o.dx10_msaa)
		RCache.set_ZB(RImplementation.Target->rt_MSAADepth->pZRT);
#endif

	
	{
		PIX_EVENT(DEFER_TEST_LIGHT_VIS);

		// perform tests
		size_t count = 0;
		light_Package& LP = Lights.package;

		// stats
		stats.l_shadowed	= LP.v_shadowed.size();
		stats.l_unshadowed	= LP.v_point.size() + LP.v_spot.size();
		stats.l_total		= stats.l_shadowed + stats.l_unshadowed;

		// perform tests
		count = std::max(count, LP.v_point.size());
		count = std::max(count, LP.v_spot.size());
		count = std::max(count, LP.v_shadowed.size());
		for (u32 it = 0; it < count; it++)
		{
			if (it < LP.v_point.size())
			{
				light* pLight = LP.v_point[it];
				pLight->vis_prepare();
				if (pLight->vis.pending)
					LP_pending.v_point.push_back(pLight);
				else
					LP_normal.v_point.push_back(pLight);
			}
			if (it < LP.v_spot.size())
			{
				light* pLight = LP.v_spot[it];
				pLight->vis_prepare();
				if (pLight->vis.pending)
					LP_pending.v_spot.push_back(pLight);
				else
					LP_normal.v_spot.push_back(pLight);
			}
			if (it < LP.v_shadowed.size())
			{
				light* pLight = LP.v_shadowed[it];
				pLight->vis_prepare();
				if (pLight->vis.pending)
					LP_pending.v_shadowed.push_back(pLight);
				else
					LP_normal.v_shadowed.push_back(pLight);
			}
		}
	}
	LP_normal.sort();
	LP_pending.sort();
	Device.Statistic->Render_CRenderRender_LightVisibility.End();

	// Active item and wallmarks
	{
		ScopeStatTimer itemUIWallmarksTimer(Device.Statistic->Render_CRenderRender_ItemUIWallmarks);
		if (g_hud && g_hud->RenderActiveItemUIQuery())
		{
			Target->phase_wallmarks();
			r_dsgraph_render_hud_ui();
		}

		// Wallmarks
		if (Wallmarks)
		{
			PIX_EVENT(DEFER_WALLMARKS);
			Target->phase_wallmarks();
			Wallmarks->Render(); // Wallmarks has priority as normal geometry
		}
	}

	// Update incremental shadowmap-visibility solver
	{
		ScopeStatTimer flushOcclusionTimer(Device.Statistic->Render_CRenderRender_FlushOcclusion);
		PIX_EVENT(DEFER_FLUSH_OCCLUSION);
		for (u32 it = 0; it < Lights_LastFrame.size(); it++)
		{
			if (0 == Lights_LastFrame[it])
				continue;

			try
			{
				Lights_LastFrame[it]->svis.flushoccq();
			}
			catch (...)
			{
				Msg("! Failed to flush-OCCq on light [%d] %X", it, *(u32*)(&Lights_LastFrame[it]));
			}
		}
		Lights_LastFrame.clear();
	}

#ifdef USE_DX11
	Device.Statistic->Render_CRenderRender_MSAA_Rain.Begin();
    // full screen pass to mark msaa-edge pixels in highest stencil bit
    if (RImplementation.o.dx10_msaa)
    {
        PIX_EVENT(MARK_MSAA_EDGES);
        Target->mark_msaa_edges();
    }

    if (ps_r3_flags.test(R3_FLAG_DYN_WET_SURF))
    {
        PIX_EVENT(DEFER_RAIN);
        render_rain();
    }
	Device.Statistic->Render_CRenderRender_MSAA_Rain.End();
#endif

	// Directional light - sun
	if (bSUN)	
	{
		ScopeStatTimer sunTimer(Device.Statistic->Render_CRenderRender_Sun);
		PIX_EVENT(DEFER_SUN);
		RImplementation.stats.l_visible++;
		if (!ps_r_flags.is(R_FLAG_SUN_OLD))
			render_sun_cascades();
		else
		{
			render_sun_near();
			render_sun();
			render_sun_filtered();
		}
		Target->accum_direct_blend();
	}

	{
		ScopeStatTimer lightTimer(Device.Statistic->Render_CRenderRender_LightRender);
		PIX_EVENT(DEFER_SELF_ILLUM);
		Target->phase_accumulator();

		// Render emissive geometry, stencil - write 0x0 at pixel pos
		RCache.set_xform_project(Device.mProject);
		RCache.set_xform_view(Device.mView);

		// Stencil - write 0x1 at pixel pos
#ifdef USE_DX11
		if (!RImplementation.o.dx10_msaa)
			RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
		else
			RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
#else
		RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
#endif

		RCache.set_CullMode(CULL_CCW);
		RCache.set_ColorWriteEnable();
		RImplementation.r_dsgraph_render_emissive();

		// Lighting, non dependant on OCCQ
		{
			PIX_EVENT(DEFER_LIGHT_NO_OCCQ);
			HOM.Disable();
			render_lights(LP_normal);
		}
        
		// Lighting, dependant on OCCQ
		{
			PIX_EVENT(DEFER_LIGHT_OCCQ);
			render_lights(LP_pending);
		}
	}
    
	// Postprocess
	{
		ScopeStatTimer lightTimer(Device.Statistic->Render_CRenderRender_Combine);
		PIX_EVENT(DEFER_LIGHT_COMBINE);
		Target->phase_combine();
	}
	VERIFY(mapDistort.empty());
}

void CRender::render_forward()
{
	VERIFY(mapDistort.empty());
	RImplementation.o.distortion = RImplementation.o.distortion_enabled; // Enable distorion

	//******* Main render - second order geometry (the one, that doesn't support deffering)
	//.todo: should be done inside "combine" with estimation of of luminance, tone-mapping, etc.
	{
		// Level
		r_pmask(false, true); // enable priority "1"
		phase = PHASE_NORMAL;
		render_main(CastToGSCMatrix(Device.mFullTransform));

		//	Igor: we don't want to render old lods on next frame.
		mapLOD.clear				();
		r_dsgraph_render_graph		(1);	// normal level, secondary priority
		PortalTraverser.fade_render	();		// faded-portals
		r_dsgraph_render_sorted		();		// strict-sorted geoms

		// Render bullet tracers
		if (g_pGameLevel)
			g_pGameLevel->RenderBullets();

		// Render glows
		if (Glows && ps_r_flags.is(R_FLAG_GLOW_USE))
			Glows->Render();

		Environment().RenderLast(); // rain/thunder-bolts
	}

	RImplementation.o.distortion = FALSE; // Disable distorion
}

// Before world render
void CRender::BeforeWorldRender()
{
}

// After world and posteffects render
void CRender::AfterWorldRender()
{
	if (Device.m_SecondViewport.IsSVPFrame())
	{
		// Copy back buffer to second viewport RT
#ifdef USE_DX11 
		ID3DTexture2D* pBackBuffer = nullptr;
		HW.m_pSwapChain->GetBuffer(0, __uuidof(ID3DTexture2D), (LPVOID*)&pBackBuffer);
		HW.pContext->CopyResource(Target->rt_secondVP->pSurface, pBackBuffer);
#else
		IDirect3DSurface9* pBackBuffer = nullptr;
		HW.pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		D3DXLoadSurfaceFromSurface(Target->rt_secondVP->pRT, nullptr, nullptr, pBackBuffer, nullptr, nullptr, D3DX_DEFAULT, 0);
#endif
		pBackBuffer->Release();
	}
}
