#include "stdafx.h"
#include "../../xrEngine/DirectXMathExternal.h"

void set_viewport(ID3DDeviceContext *dev, float w, float h)
{
	static D3D_VIEWPORT viewport[1] =
	{
		0, 0, w, h, 0.f, 1.f
	};
	dev->RSSetViewports(1, viewport);
}

void CRenderTarget::phase_ssao	()
{
	u32	Offset	= 0;

	FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	HW.pContext->ClearRenderTargetView(rt_ssao_temp->pRT, ColorRGBA);
	
	// low/hi RTs
	u_setrt( rt_ssao_temp, nullptr, nullptr, nullptr);

	RCache.set_Stencil	(FALSE);

	// Compute params
	Fmatrix m_v2w;			
	m_v2w.invert(Device.mView);

	// Fill VB
	float	scale_X				= float(Device.dwWidth)	* 0.5f / float(TEX_jitter);
	float	scale_Y				= float(Device.dwHeight) * 0.5f / float(TEX_jitter);

	float _w = float(Device.dwWidth) * 0.5f;
	float _h = float(Device.dwHeight) * 0.5f;

	set_viewport(HW.pContext, _w, _h);

	// Fill vertex buffer
	FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set						( -1,  1, 0, 1, 0,		0,	scale_Y	);	pv++;
	pv->set						( -1, -1, 0, 0, 0,		0,		  0	);	pv++;
	pv->set						(  1,  1, 1, 1, 0, scale_X,	scale_Y	);	pv++;
	pv->set						(  1, -1, 1, 0, 0, scale_X,		  0	);	pv++;
	RCache.Vertex.Unlock		(4,g_combine->vb_stride);

	// Draw
	RCache.set_Element			(s_ssao->E[0]	);
	RCache.set_Geometry			(g_combine		);

	RCache.set_c				("m_v2w",			m_v2w	);
	RCache.set_c				("resolution", _w, _h, 1.0f / _w, 1.0f / _h	);


	if( !RImplementation.o.dx10_msaa )
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	else
	{
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}  

	set_viewport(HW.pContext, float(Device.dwWidth), float(Device.dwHeight));

	RCache.set_Stencil	(FALSE);
}


void CRenderTarget::phase_downsamp	()
{
	u32			Offset = 0;

    u_setrt( rt_half_depth,nullptr,nullptr,nullptr);
	FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    HW.pContext->ClearRenderTargetView(rt_half_depth->pRT, ColorRGBA);
	u32 w = Device.dwWidth;
	u32 h = Device.dwHeight;

	if (RImplementation.o.ssao_half_data)
	{
		set_viewport(HW.pContext, float(Device.dwWidth) * 0.5f, float(Device.dwHeight) * 0.5f);
		w /= 2;
		h /= 2;
	}

	RCache.set_Stencil	(FALSE);

	{
		Fmatrix m_v2w;
		m_v2w.invert(Device.mView);

		// Fill VB
		float	scale_X				= float(w)	/ float(TEX_jitter);
		float	scale_Y				= float(h)  / float(TEX_jitter);

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						( -1,  1, 0, 1, 0,		0,	scale_Y	);	pv++;
		pv->set						( -1, -1, 0, 0, 0,		0,		  0	);	pv++;
		pv->set						(  1,  1, 1, 1, 0, scale_X,	scale_Y	);	pv++;
		pv->set						(  1, -1, 1, 0, 0, scale_X,		  0	);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);

		// Draw
		RCache.set_Element			(s_ssao->E[1]	);
		RCache.set_Geometry			(g_combine		);
		RCache.set_c				("m_v2w",			m_v2w	);

		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	if (RImplementation.o.ssao_half_data)
		set_viewport(HW.pContext, float(Device.dwWidth), float(Device.dwHeight));
}