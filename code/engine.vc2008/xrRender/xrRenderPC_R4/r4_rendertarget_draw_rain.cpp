#include "stdafx.h"

void CRenderTarget::draw_rain( light &RainSetup )
{
	float	fRainFactor	= Environment().CurrentEnv->rain_density;

	// Common calc for quad-rendering
	u32		Offset;
	u32		C					= color_rgba	(255,255,255,255);
	float	_w					= float			(Device.dwWidth);
	float	_h					= float			(Device.dwHeight);
	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	float	d_Z	= EPS_S, d_W = 1.f;

	// Common constants (light-related)
	Fvector L_dir;
	TransformDirByMatrix(Device.mView, L_dir, RainSetup.direction);
	L_dir.normalize();

	Fvector W_dirX;
	TransformDirByMatrix(Device.mView, W_dirX, { 1.f, 0.f, 0.f });
	W_dirX.normalize();

	Fvector W_dirZ;
	TransformDirByMatrix(Device.mView, W_dirZ, { 0.f, 0.f, 1.f });
	W_dirZ.normalize();

	// Fill vertex buffer
	FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
	pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
	pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
	pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock		(4,g_combine->vb_stride);
	RCache.set_Geometry			(g_combine);

	// recalculate d_Z, to perform depth-clipping
	const float fRainFar = ps_r3_dyn_wet_surf_far;

	Fvector	center_pt;
	center_pt.mad(Device.vCameraPosition, Device.vCameraDirection, fRainFar);
	CastToGSCMatrix(Device.mFullTransform).transform(center_pt);
	d_Z = center_pt.z;

	// Perform lighting
	{
		// texture adjustment matrix
		float			fRange				=  1;
		float			fBias				= -0.0001;
		float			smapsize			= float(RImplementation.o.smapsize);
		float			fTexelOffs			= (.5f / smapsize);
		float			view_dimX			= float(RainSetup.X.D.maxX-RainSetup.X.D.minX)/smapsize;
		float			view_dimY			= float(RainSetup.X.D.maxX-RainSetup.X.D.minX)/smapsize;
		float			view_sx				= float(RainSetup.X.D.minX)/smapsize;
		float			view_sy				= float(RainSetup.X.D.minY)/smapsize;

		Matrix4x4 m_TexelAdjust		= 
		{
			view_dimX/2.f,							0.0f,									0.0f,		0.0f,
			0.0f,									-view_dimY/2.f,							0.0f,		0.0f,
			0.0f,									0.0f,									fRange,		0.0f,
			view_dimX/2.f + view_sx + fTexelOffs,	view_dimY/2.f + view_sy + fTexelOffs,	fBias,		1.0f
		};

		// compute xforms
		FPU::m64r();
		Matrix4x4 xf_invview;	
		xf_invview.InvertMatrixByMatrix(Device.mView);

		// shadow xform
		Matrix4x4 m_shadow;
		{
			Matrix4x4 xf_project;
			xf_project.Multiply(RainSetup.X.D.combine, m_TexelAdjust);

			m_shadow.Multiply(xf_invview, xf_project);

			FPU::m24r();
		}

		// clouds xform
		Matrix4x4 m_clouds_shadow;
		{
			static	float	w_shift		= 0;
			Matrix4x4 m_xform;
			Fvector			normal	;	normal.setHP(1,0);
			m_xform = DirectX::XMMatrixIdentity();

			Fvector			localnormal;
			TransformDirByMatrix(m_xform, localnormal, normal);
			localnormal.normalize();

			m_clouds_shadow.Multiply(m_xform, xf_invview);
			m_xform = DirectX::XMMatrixScaling(1.f, 1.f, 1.f);
			m_clouds_shadow.Multiply(m_xform, m_clouds_shadow);
			Fvector ToTranslation = localnormal.mul(w_shift);
			m_xform = DirectX::XMMatrixTranslation(ToTranslation.x, ToTranslation.y, ToTranslation.z);
			m_clouds_shadow.Multiply(m_xform, m_clouds_shadow);
		}

		// Make jitter texture
		Fvector2					j0,j1;
		float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
		float	offset				= (.5f / float(TEX_jitter));
		j0.set						(offset,offset);
		j1.set						(scale_X,scale_X).add(offset);

		// Fill vertex buffer
		FVF::TL2uv* pv				= (FVF::TL2uv*) RCache.Vertex.Lock	(4,g_combine_2UV->vb_stride,Offset);
		pv->set						(-1,	-1,	d_Z,	d_W, C, 0, 1, 0,		scale_X);	pv++;
		pv->set						(-1,	1,	d_Z,	d_W, C, 0, 0, 0,		0);	pv++;
		pv->set						(1,		-1,	d_Z,	d_W, C, 1, 1, scale_X,	scale_X);	pv++;
		pv->set						(1,		1,	d_Z,	d_W, C, 1, 0, scale_X,	0);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_2UV->vb_stride);
		RCache.set_Geometry			(g_combine_2UV);

		//	Use for intermediate results
		//	Patch normal
		if( !RImplementation.o.dx10_msaa )
			u_setrt	(rt_Accumulator,NULL,NULL,HW.pBaseZB);
		else
			u_setrt	(rt_Accumulator,NULL,NULL,rt_MSAADepth->pZRT);

		RCache.set_Element		(s_rain->E[1]);
		RCache.set_c("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c("WorldX",				W_dirX.x,W_dirX.y,W_dirX.z,0	);
		RCache.set_c("WorldZ",				W_dirZ.x,W_dirZ.y,W_dirZ.z,0	);
		RCache.set_c("m_shadow",			m_shadow						);
		RCache.set_c("m_sunmask",			m_clouds_shadow					);
		RCache.set_c("RainDensity",			fRainFactor, 0, 0, 0			);
		RCache.set_c("RainFallof",			ps_r3_dyn_wet_surf_near, ps_r3_dyn_wet_surf_far, 0, 0);

		if (!RImplementation.o.dx10_msaa)
		{
			RCache.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x01, 0);
			RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
		}
		else
		{
			// per pixel execution
			RCache.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x81, 0);
			RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

			// per sample
			if (RImplementation.o.dx10_msaa_opt)
			{
				RCache.set_Element(s_rain_msaa[0]->E[0]);
				RCache.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0);
				RCache.set_c("WorldX", W_dirX.x, W_dirX.y, W_dirX.z, 0);
				RCache.set_c("WorldZ", W_dirZ.x, W_dirZ.y, W_dirZ.z, 0);
				RCache.set_c("m_shadow", m_shadow);
				RCache.set_c("m_sunmask", m_clouds_shadow);
				RCache.set_c("RainDensity", fRainFactor, 0, 0, 0);
				RCache.set_CullMode(CULL_NONE);
				RCache.set_Stencil(TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0);
				RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
			}
			else
			{
				for( u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i )
				{
					RCache.set_Element			( s_rain_msaa[i]->E[0]);
					RCache.set_c("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
					RCache.set_c("WorldX",				W_dirX.x,W_dirX.y,W_dirX.z,0	);
					RCache.set_c("WorldZ",				W_dirZ.x,W_dirZ.y,W_dirZ.z,0	);
					RCache.set_c("m_shadow",			m_shadow						);
					RCache.set_c("m_sunmask",			m_clouds_shadow					);
					RCache.set_c("RainDensity",			fRainFactor, 0, 0, 0			);

					StateManager.SetSampleMask(u32(1) << i);
					RCache.set_CullMode(CULL_NONE);
					RCache.set_Stencil(TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0);
					RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
				}
				StateManager.SetSampleMask( 0xffffffff );
			}
		}

		//	Apply normal
		RCache.set_Element	(s_rain->E[2]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("m_shadow",			m_shadow						);
		RCache.set_c				("m_sunmask",			m_clouds_shadow					);

		u_setrt(rt_Position, NULL, NULL, RImplementation.o.dx10_msaa ? rt_MSAADepth->pZRT : HW.pBaseZB);

		if( ! RImplementation.o.dx10_msaa )
		{
			RCache.set_Stencil( TRUE, D3DCMP_EQUAL, 0x01, 0x01, 0 );
			RCache.Render		(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
		else 
		{
			// per pixel execution
			RCache.set_Stencil( TRUE, D3DCMP_EQUAL, 0x01, 0x81, 0 );
			RCache.Render		( D3DPT_TRIANGLELIST,Offset,0,4,0,2);

			// per sample
			if( RImplementation.o.dx10_msaa_opt )
			{
				RCache.set_Element	( s_rain_msaa[0]->E[1]);
				RCache.set_Stencil   ( TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0 );
				RCache.set_CullMode(CULL_NONE	);
				RCache.Render			( D3DPT_TRIANGLELIST,Offset,0,4,0,2);
			}
			else
			{
				for( u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++ i )
				{
					RCache.set_Element			(s_rain_msaa[i]->E[1]);
					StateManager.SetSampleMask ( u32(1) << i );
					RCache.set_Stencil         ( TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0 );
					RCache.set_CullMode(CULL_NONE	);
					RCache.Render					(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
				}
				StateManager.SetSampleMask( 0xffffffff );
			}
		}

		//	Apply gloss
		RCache.set_Element		(s_rain->E[3]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("m_shadow",			m_shadow						);
		RCache.set_c				("m_sunmask",			m_clouds_shadow					);

		//	It is restored automatically by a set_Element call
		//StateManager.SetColorWriteEnable( D3Dxx_COLOR_WRITE_ENABLE_ALL );
		if( ! RImplementation.o.dx10_msaa )
			u_setrt	(rt_Color,NULL,NULL,HW.pBaseZB);
		else
			u_setrt	(rt_Color,NULL,NULL,rt_MSAADepth->pZRT);

		if( ! RImplementation.o.dx10_msaa )
		{
			RCache.set_Stencil( TRUE, D3DCMP_EQUAL, 0x01, 0x01, 0 );
			RCache.Render		(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
		else 
		{
			// per pixel execution
			RCache.set_Stencil( TRUE, D3DCMP_EQUAL, 0x01, 0x81, 0 );
			RCache.Render		( D3DPT_TRIANGLELIST,Offset,0,4,0,2);

			// per sample
			if( RImplementation.o.dx10_msaa_opt )
			{
				RCache.set_Element(s_rain_msaa[0]->E[2]);
				RCache.set_Stencil( TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0 );
				RCache.Render		(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
			}
			else 
			{
				for( u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i )
				{
					RCache.set_Element		   (s_rain_msaa[i]->E[2]);
					RCache.set_Stencil         ( TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0 );
					StateManager.SetSampleMask ( u32(1) << i );
					RCache.Render					(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
				}
				StateManager.SetSampleMask( 0xffffffff );
			}
		}
	}
}
