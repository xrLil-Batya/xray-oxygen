#include "stdafx.h"

void CRenderTarget::ProcessFXAA()
{
	float _w = float(Device.dwWidth);
	float _h = float(Device.dwHeight);

	// Luminance pass
	RenderScreenQuad(_w, _h, rt_Generic_0, s_pp_antialiasing->E[0]);

	// Main pass
#if defined(USE_DX10) || defined(USE_DX11)
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

	RenderScreenQuad(_w, _h, rt_Generic_2, s_pp_antialiasing->E[1]);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#else
	RenderScreenQuad(_w, _h, rt_Color, s_pp_antialiasing->E[1]);
#endif
}

void CRenderTarget::ProcessSMAA()
{
#if RENDER == R_R2
	u32 Offset;
	Fvector2 p0,p1;

	struct v_simple
	{
		Fvector4 p;
		Fvector2 uv0;
		Fvector4 uv1;
		Fvector4 uv2;
		Fvector4 uv3;
		Fvector4 uv4;
	};

	float _w	= float(Device.dwWidth);
	float _h	= float(Device.dwHeight);
	float ddw	= 1.f/_w;
	float ddh	= 1.f/_h;
	p0.set(0.5f/_w, 0.5f/_h);
	p1.set((_w+0.5f)/_w, (_h+0.5f)/_h);

	// Phase 0: edge detection ////////////////////////////////////////////////
	u_setrt				(rt_smaa_edgetex, nullptr, nullptr, HW.pBaseZB);
	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(TRUE, D3DCMP_ALWAYS, 1, 0, 0, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
	RCache.Clear		(0L, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L);

	// Fill vertex buffer
	v_simple* pv = (v_simple*)RCache.Vertex.Lock(4, g_smaa->vb_stride, Offset);
	pv->p.set(EPS,	    _h + EPS, EPS, 1.0f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw, p1.y, p0.x, p1.y-ddh);pv->uv2.set(p0.x+ddw, p1.y, p0.x, p1.y+ddh);pv->uv3.set(p0.x-2*ddw, p1.y, p0.x, p1.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p1.y, p0.x, p1.y+2*ddh);pv++;
	pv->p.set(EPS,	    EPS,	  EPS, 1.0f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw, p0.y, p0.x, p0.y-ddh);pv->uv2.set(p0.x+ddw, p0.y, p0.x, p0.y+ddh);pv->uv3.set(p0.x-2*ddw, p0.y, p0.x, p0.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p0.y, p0.x, p0.y+2*ddh);pv++;
	pv->p.set(_w + EPS, _h + EPS, EPS, 1.0f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw, p1.y, p1.x, p1.y-ddh);pv->uv2.set(p1.x+ddw, p1.y, p1.x, p1.y+ddh);pv->uv3.set(p1.x-2*ddw, p1.y, p1.x, p1.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p1.y, p1.x, p1.y+2*ddh);pv++;
	pv->p.set(_w + EPS, EPS,	  EPS, 1.0f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw, p0.y, p1.x, p0.y-ddh);pv->uv2.set(p1.x+ddw, p0.y, p1.x, p0.y+ddh);pv->uv3.set(p1.x-2*ddw, p0.y, p1.x, p0.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p0.y, p1.x, p0.y+2*ddh);pv++;	
	RCache.Vertex.Unlock(4, g_smaa->vb_stride);

	// Draw COLOR
	RCache.set_Element	(s_pp_antialiasing->E[2]);
	RCache.set_Geometry	(g_smaa);
	RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	// Phase 1: blend weights calculation ////////////////////////////////////
	u_setrt				(rt_smaa_blendtex, nullptr, nullptr, HW.pBaseZB);
	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(TRUE, D3DCMP_EQUAL, 1, 0, 0, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
	RCache.Clear		(0L, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L);

	// Fill vertex buffer
	pv = (v_simple*)RCache.Vertex.Lock(4, g_smaa->vb_stride, Offset);
	pv->p.set(EPS,	    _h + EPS, EPS, 1.0f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw, p1.y, p0.x, p1.y-ddh);pv->uv2.set(p0.x+ddw, p1.y, p0.x, p1.y+ddh);pv->uv3.set(p0.x-2*ddw, p1.y, p0.x, p1.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p1.y, p0.x, p1.y+2*ddh);pv++;
	pv->p.set(EPS,	    EPS,	  EPS, 1.0f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw, p0.y, p0.x, p0.y-ddh);pv->uv2.set(p0.x+ddw, p0.y, p0.x, p0.y+ddh);pv->uv3.set(p0.x-2*ddw, p0.y, p0.x, p0.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p0.y, p0.x, p0.y+2*ddh);pv++;
	pv->p.set(_w + EPS, _h + EPS, EPS, 1.0f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw, p1.y, p1.x, p1.y-ddh);pv->uv2.set(p1.x+ddw, p1.y, p1.x, p1.y+ddh);pv->uv3.set(p1.x-2*ddw, p1.y, p1.x, p1.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p1.y, p1.x, p1.y+2*ddh);pv++;
	pv->p.set(_w + EPS, EPS,	  EPS, 1.0f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw, p0.y, p1.x, p0.y-ddh);pv->uv2.set(p1.x+ddw, p0.y, p1.x, p0.y+ddh);pv->uv3.set(p1.x-2*ddw, p0.y, p1.x, p0.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p0.y, p1.x, p0.y+2*ddh);pv++;	
	RCache.Vertex.Unlock(4, g_smaa->vb_stride);

	// Draw COLOR
	RCache.set_Element	(s_pp_antialiasing->E[3]);
	RCache.set_Geometry	(g_smaa);
	RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	// Phase 2: neighbour blend //////////////////////////////////////////////
	u_setrt				(rt_Generic_0, nullptr, nullptr, HW.pBaseZB);
	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(FALSE);

	// Fill vertex buffer
	pv = (v_simple*)RCache.Vertex.Lock(4, g_smaa->vb_stride, Offset);
	pv->p.set(EPS,	    _h + EPS, EPS, 1.0f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw, p1.y, p0.x, p1.y-ddh);pv->uv2.set(p0.x+ddw, p1.y, p0.x, p1.y+ddh);pv->uv3.set(p0.x-2*ddw, p1.y, p0.x, p1.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p1.y, p0.x, p1.y+2*ddh);pv++;
	pv->p.set(EPS,	    EPS,	  EPS, 1.0f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw, p0.y, p0.x, p0.y-ddh);pv->uv2.set(p0.x+ddw, p0.y, p0.x, p0.y+ddh);pv->uv3.set(p0.x-2*ddw, p0.y, p0.x, p0.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p0.y, p0.x, p0.y+2*ddh);pv++;
	pv->p.set(_w + EPS, _h + EPS, EPS, 1.0f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw, p1.y, p1.x, p1.y-ddh);pv->uv2.set(p1.x+ddw, p1.y, p1.x, p1.y+ddh);pv->uv3.set(p1.x-2*ddw, p1.y, p1.x, p1.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p1.y, p1.x, p1.y+2*ddh);pv++;
	pv->p.set(_w + EPS, EPS,	  EPS, 1.0f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw, p0.y, p1.x, p0.y-ddh);pv->uv2.set(p1.x+ddw, p0.y, p1.x, p0.y+ddh);pv->uv3.set(p1.x-2*ddw, p0.y, p1.x, p0.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p0.y, p1.x, p0.y+2*ddh);pv++;	
	RCache.Vertex.Unlock(4, g_smaa->vb_stride);

	// Draw COLOR
	RCache.set_Element	(s_pp_antialiasing->E[4]);
	RCache.set_Geometry	(g_smaa);
	RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
#endif
}

void CRenderTarget::PhaseAA()
{
	if(ps_r_pp_aa_mode)
	{
		switch (ps_r_pp_aa_mode)
		{
		case FXAA: ProcessFXAA(); break;
		case SMAA: ProcessSMAA(); break;
		case DLAA: ProcessDLAA(); break;
		}
		
		float _w = float(Device.dwWidth);
		float _h = float(Device.dwHeight);

	// Temportal AA passing
#if defined(USE_DX10) || defined(USE_DX11)
		ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;
	
		RenderScreenQuad(_w, _h, rt_Generic_2, s_pp_taa->E[0]);
		HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#endif
	}
}

void CRenderTarget::ProcessDLAA()
{
	float _w = float(Device.dwWidth);
	float _h = float(Device.dwHeight);

	// Pass 0
#if defined(USE_DX10) || defined(USE_DX11)
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

	RenderScreenQuad(_w, _h, rt_Generic_2, s_pp_antialiasing->E[3]);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#else
	RenderScreenQuad(_w, _h, rt_Color, s_pp_antialiasing->E[3]);
#endif
}
