#include "stdafx.h"
#include "../../xrEngine/igame_persistent.h"
#include "../../xrEngine/environment.h"
#include "../../xrEngine/DirectXMathExternal.h"

//////////////////////////////////////////////////////////////////////////
// tables to calculate view-frustum bounds in world space
// note: D3D uses [0..1] range for Z
constexpr static Fvector3 corners[8] =
{
	{ -1, -1,  0.7 },	{ -1, -1, +1},
	{ -1, +1, +1 },		{ -1, +1,  0.7},
	{ +1, +1, +1 },		{ +1, +1,  0.7},
	{ +1, -1, +1 },		{ +1, -1,  0.7}
};

constexpr static u16 facetable[16][3] =
{
	{ 3, 2, 1 },
	{ 3, 1, 0 },
	{ 7, 6, 5 },
	{ 5, 6, 4 },
	{ 3, 5, 2 },
	{ 4, 2, 5 },
	{ 1, 6, 7 },
	{ 7, 0, 1 },

	{ 5, 3, 0 },
	{ 7, 5, 0 },

	{ 1, 4, 6 },
	{ 2, 4, 1 },
};

// True bias for direct lighting 
inline float TrueBias = -0.01f;

void CRenderTarget::accum_direct(u32 sub_phase)
{
	// Choose normal code-path or filtered
	phase_accumulator();
	if (RImplementation.o.sunfilter) {
		accum_direct_f(sub_phase);
		return;
	}

	//	choose corect element for the sun shader
	u32 uiElementIndex = sub_phase;
	if ((uiElementIndex == SE_SUN_NEAR) && use_minmax_sm_this_frame())
		uiElementIndex = SE_SUN_NEAR_MINMAX;

	//	TODO: DX10: Remove half pixe offset
	// *** assume accumulator setted up ***
	light* fuckingsun = (light*)RImplementation.Lights.sun._get();

	// Common calc for quad-rendering
	u32		Offset;
	u32		C = color_rgba(255, 255, 255, 255);
	float	_w = float(Device.dwWidth);
	float	_h = float(Device.dwHeight);
	Fvector2					p0, p1;
	p0.set(.5f / _w, .5f / _h);
	p1.set((_w + .5f) / _w, (_h + .5f) / _h);
	float	d_Z = EPS_S, d_W = 1.f;

	// Common constants (light-related)
	Fvector		L_dir, L_clr;	float L_spec;
	L_clr.set(fuckingsun->color.r, fuckingsun->color.g, fuckingsun->color.b);
	L_spec = Diffuse::u_diffuse2s(L_clr);
	Device.mView.transform_dir(L_dir, fuckingsun->direction);
	L_dir.normalize();

	// Perform masking (only once - on the first/near phase)
	RCache.set_CullMode(CULL_NONE);
	PIX_EVENT(SE_SUN_NEAR_sub_phase);

	if (SE_SUN_NEAR == sub_phase)
	{
		// Fill vertex buffer
		FVF::TL* pv = (FVF::TL*)	RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
		pv->set(EPS, float(_h + EPS), d_Z, d_W, C, p0.x, p1.y);	pv++;
		pv->set(EPS, EPS, d_Z, d_W, C, p0.x, p0.y);	pv++;
		pv->set(float(_w + EPS), float(_h + EPS), d_Z, d_W, C, p1.x, p1.y);	pv++;
		pv->set(float(_w + EPS), EPS, d_Z, d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock(4, g_combine->vb_stride);
		RCache.set_Geometry(g_combine);

		// setup
		float	intensity = 0.3f * fuckingsun->color.r + 0.48f * fuckingsun->color.g + 0.22f * fuckingsun->color.b;
		Fvector	dir = L_dir;
		dir.normalize().mul(-_sqrt(intensity + EPS));
		RCache.set_Element(s_accum_mask->E[SE_MASK_DIRECT]);		// masker
		RCache.set_c("Ldynamic_dir", dir.x, dir.y, dir.z, 0);

		if (!RImplementation.o.dx10_msaa)
		{
			RCache.set_Stencil(TRUE, D3D11_COMPARISON_LESS_EQUAL, dwLightMarkerID, 0x01, 0xff, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_KEEP);
			RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
		}
		else
		{
			// per pixel rendering // checked Holger
			RCache.set_Stencil(TRUE, D3D11_COMPARISON_EQUAL, dwLightMarkerID, 0x81, 0x7f, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_KEEP);
			RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

			// per sample rendering
			if (RImplementation.o.dx10_msaa_opt)
			{
				RCache.set_Element(s_accum_mask_msaa[0]->E[SE_MASK_DIRECT]);		// masker
				RCache.set_CullMode(CULL_NONE);
				RCache.set_Stencil(TRUE, D3D11_COMPARISON_EQUAL, dwLightMarkerID | 0x80, 0x81, 0x7f, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_KEEP);
				RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
			}
			else
			{
				for (u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i)
				{
					RCache.set_Element(s_accum_mask_msaa[i]->E[SE_MASK_DIRECT]);		// masker
					RCache.set_CullMode(CULL_NONE);
					RCache.set_Stencil(TRUE, D3D11_COMPARISON_EQUAL, dwLightMarkerID | 0x80, 0x81, 0x7f, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_KEEP);
					StateManager.SetSampleMask(u32(1) << i);
					RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
				}
				StateManager.SetSampleMask(0xffffffff);
			}
			RCache.set_Stencil(TRUE, D3D11_COMPARISON_LESS_EQUAL, dwLightMarkerID, 0x01, 0xff, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_KEEP);
		}
	}

	// recalculate d_Z, to perform depth-clipping
	Fvector	center_pt;			center_pt.mad(Device.vCameraPosition, Device.vCameraDirection, ps_r_sun_near);
	Device.mFullTransform.transform(center_pt);
	d_Z = center_pt.z;

	PIX_EVENT(Perform_lighting);

	// Perform lighting
	{
		phase_accumulator();
		RCache.set_CullMode(CULL_NONE);
		RCache.set_ColorWriteEnable();

		float			fRange = (SE_SUN_NEAR == sub_phase) ? ps_r_sun_depth_near_scale : ps_r_sun_depth_far_scale;

		//	TODO: DX10: Remove this when fix inverse culling for far region
		float			fBias = (SE_SUN_NEAR == sub_phase) ? (-ps_r_sun_depth_near_bias) : ps_r_sun_depth_far_bias;
		Matrix4x4 m_TexelAdjust =
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f,				0.5f,				fBias,			1.0f
		};

		// compute xforms
		Fmatrix				xf_invview;		xf_invview.invert(Device.mView);

		// shadow xform
		Fmatrix				m_shadow;
		{
			Fmatrix xf_project;
			xf_project.mul(m_TexelAdjust, fuckingsun->X.D.combine);
			m_shadow.mul(xf_project, xf_invview);

			// tsm-bias
			if (SE_SUN_FAR == sub_phase)
			{
				Fvector		bias;	bias.mul(L_dir, TrueBias);
				Fmatrix		bias_t;	bias_t.translate(bias);
				m_shadow.mulB_44(bias_t);
			}
		}

		// clouds xform
		Fmatrix				m_clouds_shadow;
		{
			static	float	w_shift = 0;
			Fmatrix			m_xform;
			Fvector			direction = fuckingsun->direction;
			float	w_dir = Environment().CurrentEnv->wind_direction;
			Fvector			normal;	normal.setHP(w_dir, 0);
			w_shift += 0.003f * Device.fTimeDelta;
			Fvector			position;	position.set(0, 0, 0);
			m_xform.build_camera_dir(position, direction, normal);
			Fvector			localnormal; m_xform.transform_dir(localnormal, normal); localnormal.normalize();
			m_clouds_shadow.mul(m_xform, xf_invview);
			m_xform.scale(0.002f, 0.002f, 1.f);
			m_clouds_shadow.mulA_44(m_xform);
			m_xform.translate(localnormal.mul(w_shift));
			m_clouds_shadow.mulA_44(m_xform);
		}

		// Make jitter texture
		Fvector2					j0, j1;
		float	scale_X = float(Device.dwWidth) / float(TEX_jitter);
		float	offset = (.5f / float(TEX_jitter));
		j0.set(offset, offset);
		j1.set(scale_X, scale_X).add(offset);

		// Fill vertex buffer
		FVF::TL2uv* pv = (FVF::TL2uv*) RCache.Vertex.Lock(4, g_combine_2UV->vb_stride, Offset);
		pv->set(-1, -1, d_Z, d_W, C, 0, 1, 0, scale_X);	pv++;
		pv->set(-1, 1, d_Z, d_W, C, 0, 0, 0, 0);	pv++;
		pv->set(1, -1, d_Z, d_W, C, 1, 1, scale_X, scale_X);	pv++;
		pv->set(1, 1, d_Z, d_W, C, 1, 0, scale_X, 0);	pv++;
		RCache.Vertex.Unlock(4, g_combine_2UV->vb_stride);
		RCache.set_Geometry(g_combine_2UV);

		// setup
		RCache.set_Element(s_accum_direct->E[uiElementIndex]);
		RCache.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0);
		RCache.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
		RCache.set_c("m_shadow", m_shadow);
		RCache.set_c("m_sunmask", m_clouds_shadow);

		// setup stencil
		if (!RImplementation.o.dx10_msaa)
		{
			RCache.set_Stencil(TRUE, D3D11_COMPARISON_LESS_EQUAL, dwLightMarkerID, 0xff, 0x00);
			RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
		}
		else
		{
			// per pixel
			RCache.set_Stencil(TRUE, D3D11_COMPARISON_EQUAL, dwLightMarkerID, 0xff, 0x00);
			RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

			// per sample
			if (RImplementation.o.dx10_msaa_opt)
			{
				RCache.set_Element(s_accum_direct_msaa[0]->E[uiElementIndex]);
				RCache.set_Stencil(TRUE, D3D11_COMPARISON_EQUAL, dwLightMarkerID | 0x80, 0xff, 0x00);
				RCache.set_CullMode(CULL_NONE);
				RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
			}
			else
			{
				for (u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i)
				{
					RCache.set_Element(s_accum_direct_msaa[i]->E[uiElementIndex]);
					RCache.set_Stencil(TRUE, D3D11_COMPARISON_EQUAL, dwLightMarkerID | 0x80, 0xff, 0x00);
					RCache.set_CullMode(CULL_NONE);
					StateManager.SetSampleMask(u32(1) << i);
					RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
				}
				StateManager.SetSampleMask(0xffffffff);
			}
			RCache.set_Stencil(TRUE, D3D11_COMPARISON_LESS_EQUAL, dwLightMarkerID, 0xff, 0x00);
		}
	}
}

void CRenderTarget::accum_direct_cascade	( u32 sub_phase, Fmatrix& xform, Fmatrix& xform_prev, float fBias )
{
	// Choose normal code-path or filtered
	phase_accumulator					();
	if (RImplementation.o.sunfilter)	{
		accum_direct_f	(sub_phase);
		return			;
	}

	//	choose correct element for the sun shader
	u32 uiElementIndex = sub_phase;
	if ( (uiElementIndex==SE_SUN_NEAR) && use_minmax_sm_this_frame())
		uiElementIndex = SE_SUN_NEAR_MINMAX;

	//	TODO: DX10: Remove half pixe offset
	// *** assume accumulator setted up ***
	light*			fuckingsun			= (light*)RImplementation.Lights.sun._get()	;

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
	Fvector		L_dir,L_clr;	float L_spec;
	L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
	L_spec						= Diffuse::u_diffuse2s	(L_clr);
	Device.mView.transform_dir	(L_dir,fuckingsun->direction);
	L_dir.normalize				();

	// Perform masking (only once - on the first/near phase)
	RCache.set_CullMode			(CULL_NONE	);
	PIX_EVENT(SE_SUN_NEAR_sub_phase);
	if (SE_SUN_NEAR==sub_phase)	//.
		//if( 0 )
	{
		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// setup
		float	intensity			= 0.3f*fuckingsun->color.r + 0.48f*fuckingsun->color.g + 0.22f*fuckingsun->color.b;
		Fvector	dir					= L_dir;
		dir.normalize().mul	(- _sqrt(intensity+EPS));
		RCache.set_Element			(s_accum_mask->E[SE_MASK_DIRECT]);		// masker
		RCache.set_c				("Ldynamic_dir",		dir.x,dir.y,dir.z,0		);

		if( ! RImplementation.o.dx10_msaa )
		{
			RCache.set_Stencil		(TRUE,D3D11_COMPARISON_LESS_EQUAL,dwLightMarkerID,0x01,0xff,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
		else
		{
			// per pixel rendering // checked Holger
			RCache.set_Stencil		(TRUE,D3D11_COMPARISON_EQUAL,dwLightMarkerID,0x81,0x7f,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

			// per sample rendering
			if( RImplementation.o.dx10_msaa_opt )
			{
				RCache.set_Element		(s_accum_mask_msaa[0]->E[SE_MASK_DIRECT]);		// masker
				RCache.set_CullMode		(CULL_NONE	);
				RCache.set_Stencil		(TRUE,D3D11_COMPARISON_EQUAL,dwLightMarkerID|0x80,0x81,0x7f,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
				RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
			}
			else 
			{
				for( u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i )
				{
					RCache.set_Element		   (s_accum_mask_msaa[i]->E[SE_MASK_DIRECT]);		// masker
					RCache.set_CullMode		   (CULL_NONE	);
					RCache.set_Stencil			(TRUE,D3D11_COMPARISON_EQUAL,dwLightMarkerID|0x80,0x81,0x7f,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
					StateManager.SetSampleMask ( u32(1)<<i );
					RCache.Render					(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
				}
				StateManager.SetSampleMask( 0xffffffff );
			}
			RCache.set_Stencil		(TRUE,D3D11_COMPARISON_LESS_EQUAL,dwLightMarkerID,0x01,0xff,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
		} 
	}

	// recalculate d_Z, to perform depth-clipping
	Fvector	center_pt;			center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r_sun_near);
	Device.mFullTransform.transform(center_pt)	;
	d_Z							= center_pt.z	;

	PIX_EVENT(Perform_lighting);

	// Perform lighting
	{
		phase_accumulator					()	;
		RCache.set_CullMode					(CULL_CCW); //******************************************************************
		RCache.set_ColorWriteEnable			()	;

		float			fRange				= (SE_SUN_NEAR==sub_phase)?ps_r_sun_depth_near_scale:ps_r_sun_depth_far_scale;

		//	TODO: DX10: Remove this when fix inverse culling for far region
		Matrix4x4 m_TexelAdjust		=
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f,				0.5f,				fBias,			1.0f
		};

		// compute xforms
		Fmatrix				xf_invview;		xf_invview.invert	(Device.mView);

		// shadow xform
		Fmatrix				m_shadow;
		{
			Fmatrix xf_project;
			xf_project.mul(m_TexelAdjust, fuckingsun->X.D.combine);
			m_shadow.mul(xf_project, xf_invview);

			// tsm-bias
			if (SE_SUN_FAR == sub_phase)
			{
				Fvector		bias;	bias.mul(L_dir, TrueBias);
				Fmatrix		bias_t;	bias_t.translate(bias);
				m_shadow.mulB_44(bias_t);
			}
		}

		// clouds xform
		Fmatrix				m_clouds_shadow;
		{
			static	float	w_shift		= 0;
			Fmatrix			m_xform;
			Fvector			direction	= fuckingsun->direction	;
			float	w_dir				= Environment().CurrentEnv->wind_direction	;
			Fvector			normal	;	normal.setHP(w_dir,0);
			w_shift		+=	0.003f*Device.fTimeDelta;
			Fvector			position;	position.set(0,0,0);
			m_xform.build_camera_dir	(position,direction,normal)	;
			Fvector			localnormal;m_xform.transform_dir(localnormal,normal); localnormal.normalize();
			m_clouds_shadow.mul			(m_xform,xf_invview)		;
			m_xform.scale				(0.002f,0.002f,1.f)			;
			m_clouds_shadow.mulA_44		(m_xform)					;
			m_xform.translate			(localnormal.mul(w_shift))	;
			m_clouds_shadow.mulA_44		(m_xform)					;
		}

		// Compute textgen texture for pixel shader, for possitions texture.
		Fmatrix			m_Texgen;
		m_Texgen.identity();
 		RCache.xforms.set_W( m_Texgen );
 		RCache.xforms.set_V( Device.mView );
 		RCache.xforms.set_P( Device.mProject );
 		u_compute_texgen_screen	( m_Texgen );


		// Make jitter texture
		Fvector2					j0,j1;
		float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
		float	offset				= (.5f / float(TEX_jitter));
		j0.set						(offset,offset);
		j1.set						(scale_X,scale_X).add(offset);

		// Fill vertex buffer
		u32		i_offset;
		{
			u16*	pib					= RCache.Index.Lock	(sizeof(facetable)/sizeof(u16),i_offset);
            std::memcpy(pib,&facetable,sizeof(facetable));
			RCache.Index.Unlock			(sizeof(facetable)/sizeof(u16));

			u32 ver_count = sizeof(corners)/ sizeof(Fvector3);
			Fvector4* pv				= (Fvector4*)	RCache.Vertex.Lock	( ver_count,g_combine_cuboid.stride(),Offset);
			

			Fmatrix inv_XDcombine;
			if(sub_phase == SE_SUN_FAR )
				inv_XDcombine.invert(xform_prev);
			else
				inv_XDcombine.invert(xform);
				

			for ( u32 i = 0; i < ver_count; ++i )
			{
				Fvector3 tmp_vec;
				inv_XDcombine.transform(tmp_vec, corners[i]);
				pv->set						( tmp_vec.x,tmp_vec.y, tmp_vec.z, 1 );	
				pv++;
			}
			RCache.Vertex.Unlock		(ver_count,g_combine_cuboid.stride());
		}

		RCache.set_Geometry			(g_combine_cuboid);

		// setup
		RCache.set_Element			(s_accum_direct->E[uiElementIndex]);
		RCache.set_c				("m_texgen",			m_Texgen);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z,L_spec	);
		RCache.set_c				("m_shadow",			m_shadow						);
		RCache.set_c				("m_sunmask",			m_clouds_shadow					);

		if(sub_phase == SE_SUN_FAR)
		{
			Fvector3 view_viewspace;	view_viewspace.set( 0, 0, 1 );

			m_shadow.transform_dir( view_viewspace );
			Fvector4 view_projlightspace;
			view_projlightspace.set( view_viewspace.x, view_viewspace.y, 0, 0 );
			view_projlightspace.normalize();

			RCache.set_c				("view_shadow_proj",	view_projlightspace);
		}

		// Enable Z function only for near and middle cascades, the far one is restricted by only stencil.
		if ((SE_SUN_NEAR == sub_phase || SE_SUN_MIDDLE == sub_phase))
			RCache.set_ZFunc(D3D11_COMPARISON_GREATER_EQUAL);
		else
		{
			if (!ps_r_flags.is(R_FLAG_SUN_ZCULLING))
				RCache.set_ZFunc(D3D11_COMPARISON_ALWAYS);
			else
				RCache.set_ZFunc(D3D11_COMPARISON_LESS);
		}

		u32 st_mask = 0xFE;
		D3D11_STENCIL_OP st_pass = D3D11_STENCIL_OP_ZERO;

		if( sub_phase == SE_SUN_FAR )
		{
			st_mask = 0x00;
			st_pass = D3D11_STENCIL_OP_KEEP;
		}

		// setup stencil
		if( ! RImplementation.o.dx10_msaa )
		{
			RCache.set_Stencil	(TRUE,D3D11_COMPARISON_LESS_EQUAL,dwLightMarkerID,0xff,st_mask, D3D11_STENCIL_OP_KEEP, st_pass, D3D11_STENCIL_OP_KEEP);
			RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,8,0,16);
		}
		else 
		{
			// per pixel
			RCache.set_Stencil	(TRUE,D3D11_COMPARISON_EQUAL,dwLightMarkerID,0xff,st_mask, D3D11_STENCIL_OP_KEEP, st_pass, D3D11_STENCIL_OP_KEEP);
			RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,8,0,16);

			// per sample
			if( RImplementation.o.dx10_msaa_opt )
			{
				RCache.set_Element	(s_accum_direct_msaa[0]->E[uiElementIndex]);

				if( (SE_SUN_NEAR==sub_phase || SE_SUN_MIDDLE==sub_phase) )
					RCache.set_ZFunc(D3D11_COMPARISON_GREATER_EQUAL);
				else
				{
					if (!ps_r_flags.is(R_FLAG_SUN_ZCULLING))
						RCache.set_ZFunc(D3D11_COMPARISON_ALWAYS);
					else
						RCache.set_ZFunc(D3D11_COMPARISON_LESS);
				}


				RCache.set_Stencil	(TRUE,D3D11_COMPARISON_EQUAL,dwLightMarkerID|0x80,0xff,st_mask, D3D11_STENCIL_OP_KEEP, st_pass, D3D11_STENCIL_OP_KEEP);
				RCache.set_CullMode	(CULL_NONE	);
				RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,8,0,16);
			}
			else
			{
				for( u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i )
				{
					RCache.set_Element			(s_accum_direct_msaa[i]->E[uiElementIndex]);

					if( (SE_SUN_NEAR==sub_phase || SE_SUN_MIDDLE==sub_phase) )
						RCache.set_ZFunc(D3D11_COMPARISON_GREATER_EQUAL);
					else
					{
						if (!ps_r_flags.is(R_FLAG_SUN_ZCULLING))
							RCache.set_ZFunc(D3D11_COMPARISON_ALWAYS);
						else
							RCache.set_ZFunc(D3D11_COMPARISON_LESS);
					}

					RCache.set_Stencil	      (TRUE,D3D11_COMPARISON_EQUAL,dwLightMarkerID|0x80,0xff,st_mask, D3D11_STENCIL_OP_KEEP, st_pass, D3D11_STENCIL_OP_KEEP);
					RCache.set_CullMode		   (CULL_NONE	);
					StateManager.SetSampleMask ( u32(1) << i  );
					RCache.Render					(D3DPT_TRIANGLELIST,Offset,0,8,0,16);
				}
				StateManager.SetSampleMask	( 0xffffffff );
			}
			RCache.set_Stencil	(TRUE,D3D11_COMPARISON_LESS_EQUAL,dwLightMarkerID,0xff,0x00);
		}
	}
}

void CRenderTarget::accum_direct_f		(u32 sub_phase)
{
	PIX_EVENT(accum_direct_f);

	phase_accumulator();
	RImplementation.o.dx10_msaa ? u_setrt(rt_Generic_0_r,nullptr,nullptr,RImplementation.Target->rt_MSAADepth->pZRT)
								: u_setrt(rt_Generic_0,nullptr,nullptr,HW.pBaseZB);

	// *** assume accumulator setted up ***
	light*			fuckingsun			= (light*)RImplementation.Lights.sun._get()	;

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
	Fvector		L_dir,L_clr;	float L_spec;
	L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
	L_spec						= Diffuse::u_diffuse2s	(L_clr);
	Device.mView.transform_dir	(L_dir,fuckingsun->direction);
	L_dir.normalize				();

	// Perform masking (only once - on the first/near phase)
	RCache.set_CullMode			(CULL_NONE	);
	if (SE_SUN_NEAR==sub_phase)	//.
	{
		// For sun-filter - clear to zero
		FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		HW.pContext->ClearRenderTargetView(RCache.get_RT(), ColorRGBA);

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// setup
		float	intensity			= 0.3f*fuckingsun->color.r + 0.48f*fuckingsun->color.g + 0.22f*fuckingsun->color.b;
		Fvector	dir					= L_dir;
		dir.normalize().mul	(- _sqrt(intensity+EPS));
		RCache.set_Element			(s_accum_mask->E[SE_MASK_DIRECT]);		// masker
		RCache.set_c				("Ldynamic_dir",		dir.x,dir.y,dir.z,0		);

      if( ! RImplementation.o.dx10_msaa )
      {
		   RCache.set_Stencil	(TRUE,D3D11_COMPARISON_LESS_EQUAL,dwLightMarkerID,0x01,0xff,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
		   RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
      }
      else 
      {
		   // per pixel
		   RCache.set_Stencil	(TRUE,D3D11_COMPARISON_EQUAL,dwLightMarkerID,0x81,0x7f,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
		   RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
   		
		   // per sample
         if( RImplementation.o.dx10_msaa_opt )
         {
		      RCache.set_Element	(s_accum_mask_msaa[0]->E[SE_MASK_DIRECT]);		// masker
            RCache.set_Stencil	(TRUE,D3D11_COMPARISON_LESS,dwLightMarkerID,0x81,0x7f,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
	         RCache.set_CullMode	(CULL_NONE	);
            RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
         }
         else 
         {
		      for( u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i )
		      {
			      RCache.set_Element		   (s_accum_mask_msaa[i]->E[SE_MASK_DIRECT]);		// masker
               RCache.set_Stencil	      (TRUE,D3D11_COMPARISON_LESS,dwLightMarkerID,0x81,0x7f,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
	            RCache.set_CullMode	      (CULL_NONE	);
               StateManager.SetSampleMask ( u32(1) << i );
               RCache.Render					(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		      }
		      StateManager.SetSampleMask( 0xffffffff );
         }	
		   RCache.set_Stencil	(TRUE,D3D11_COMPARISON_LESS_EQUAL,dwLightMarkerID,0x01,0xff,D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_REPLACE,D3D11_STENCIL_OP_KEEP);
      }
	}

	// recalculate d_Z, to perform depth-clipping
	Fvector	center_pt;			center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r_sun_near);
	Device.mFullTransform.transform(center_pt)	;
	d_Z							= center_pt.z	;

	// Perform lighting
	{
		if( ! RImplementation.o.dx10_msaa )
			u_setrt								(rt_Generic_0,nullptr, nullptr,HW.pBaseZB);  // enshure RT setup
		else
			u_setrt								(rt_Generic_0_r, nullptr, nullptr,RImplementation.Target->rt_MSAADepth->pZRT);  // enshure RT setup
		RCache.set_CullMode					(CULL_NONE	);
		RCache.set_ColorWriteEnable			();

		// texture adjustment matrix
		float			fTexelOffs			= (.5f / float(RImplementation.o.smapsize));
		float			fRange				= (SE_SUN_NEAR==sub_phase)?ps_r_sun_depth_near_scale:ps_r_sun_depth_far_scale;
		//	TODO: DX10: Remove this when fix inverse culling for far region
		float fBias = (SE_SUN_NEAR==sub_phase)?ps_r_sun_depth_near_bias:-ps_r_sun_depth_far_bias;
		Fmatrix m_TexelAdjust		=
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
		};

		// compute xforms
		Fmatrix m_shadow;
		{
			Fmatrix xf_invview;
			xf_invview.invert(Device.mView);

			Fmatrix xf_project;		
			xf_project.mul(m_TexelAdjust, fuckingsun->X.D.combine);
			m_shadow.mul(xf_project, xf_invview);

			// tsm-bias
			if (SE_SUN_FAR == sub_phase)
			{
				Fvector bias;	
				bias.mul(L_dir, fBias);

				Fmatrix bias_t; bias_t.translate(bias.x, bias.y, bias.z);

				m_shadow.mul(m_shadow, bias_t);
			}
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

		// setup
		RCache.set_Element			(s_accum_direct->E[sub_phase]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z,L_spec	);
		RCache.set_c				("m_shadow",			m_shadow						);

      if( ! RImplementation.o.dx10_msaa )
      {
		   // setup stencil
		   RCache.set_Stencil	(TRUE,D3D11_COMPARISON_LESS_EQUAL,dwLightMarkerID,0xff,0x00);
		   RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
      }
      else
      {
		   // per pixel
		   RCache.set_Stencil	(TRUE,D3D11_COMPARISON_EQUAL,dwLightMarkerID,0xff,0x00);
		   RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
   		
         // per sample // checked Holger
         if( RImplementation.o.dx10_msaa_opt )
         {
		      RCache.set_Element	(s_accum_direct_msaa[0]->E[sub_phase]);
	         RCache.set_CullMode	(CULL_NONE	);
            RCache.set_Stencil	(TRUE,D3D11_COMPARISON_LESS,dwLightMarkerID|0x80,0xff,0x00);
		      RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
         }
         else
         {
		      for( u32 i = 0; i < RImplementation.o.dx10_msaa_samples; ++i )
		      {
			      RCache.set_Element	      (s_accum_direct_msaa[i]->E[sub_phase]);
	            RCache.set_CullMode	      (CULL_NONE	);
               RCache.set_Stencil	      (TRUE,D3D11_COMPARISON_LESS,dwLightMarkerID|0x80,0xff,0x00);
               StateManager.SetSampleMask ( u32(1)<< i );
			      RCache.Render				   (D3DPT_TRIANGLELIST,Offset,0,4,0,2);			
		      }
		      StateManager.SetSampleMask( 0xffffffff );
         }
		   RCache.set_Stencil	(TRUE,D3D11_COMPARISON_LESS_EQUAL,dwLightMarkerID,0xff,0x00);
      }
	}
}