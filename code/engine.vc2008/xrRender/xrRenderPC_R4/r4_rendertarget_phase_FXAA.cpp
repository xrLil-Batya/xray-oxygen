#include "stdafx.h"
#include "r4_rendertarget.h"

void CRenderTarget::phase_fxaa()
{
    u32 Offset = 0;
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    float du = ps_r_pps_u, dv = ps_r_pps_v;

	Fvector2 p0, p1;
	p0.set(0.5f/_w, 0.5f/_h);
	p1.set((_w+0.5f)/_w, (_h+0.5f)/_h);

	struct v_simple
	{
		Fvector4	p;
		Fvector2	uv0;
	};

	// Luminance pass //////////////////////////////////
    u_setrt(rt_Generic, 0, NULL, HW.pBaseZB);
	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(FALSE);
 
	v_simple* pv = (v_simple*)RCache.Vertex.Lock(4, g_fxaa->vb_stride, Offset);
    pv->p.set(du + EPS,			dv + _h + EPS,	EPS, 1.0f); pv->uv0.set(p0.x, p1.y);   pv++;
    pv->p.set(du + EPS,			dv + EPS,		EPS, 1.0f); pv->uv0.set(p0.x, p0.y);   pv++;
    pv->p.set(du + _w + EPS,	dv + _h + EPS,	EPS, 1.0f); pv->uv0.set(p1.x, p1.y);   pv++;
    pv->p.set(du + _w + EPS,	dv + EPS,		EPS, 1.0f); pv->uv0.set(p1.x, p0.y);   pv++;
    RCache.Vertex.Unlock(4, g_fxaa->vb_stride);

    RCache.set_Element(s_fxaa->E[0]);
    RCache.set_Geometry(g_fxaa);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
      
	// Main pass //////////////////////////////////
	u_setrt(rt_Generic, 0, NULL, HW.pBaseZB);
	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(FALSE);
 
    pv = (v_simple*)RCache.Vertex.Lock(4, g_fxaa->vb_stride, Offset);
    pv->p.set(du + EPS,			dv + _h + EPS,	EPS, 1.0f); pv->uv0.set(p0.x, p1.y);   pv++;
    pv->p.set(du + EPS,			dv + EPS,		EPS, 1.0f); pv->uv0.set(p0.x, p0.y);   pv++;
    pv->p.set(du + _w + EPS,	dv + _h + EPS,	EPS, 1.0f); pv->uv0.set(p1.x, p1.y);   pv++;
    pv->p.set(du + _w + EPS,	dv + EPS,		EPS, 1.0f); pv->uv0.set(p1.x, p0.y);   pv++;
    RCache.Vertex.Unlock(4, g_fxaa->vb_stride);

    RCache.set_Element(s_fxaa->E[1]);
    RCache.set_Geometry(g_fxaa);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), rt_Generic->pTexture->surface_get());
}