#include "stdafx.h"
#include "dxRainRender.h"

#include "../../xrEngine/Rain.h"

#include "D3DUtils.h"
// Declared in xrEngine/Rain.cpp
extern ENGINE_API int	max_desired_items;
extern ENGINE_API float	source_radius;
extern ENGINE_API float	source_offset;
extern ENGINE_API float	max_distance;
extern ENGINE_API float	sink_offset;
extern ENGINE_API float	drop_length;
extern ENGINE_API float	drop_width;

extern ENGINE_API int	max_particles;
extern ENGINE_API int	particles_cache;
extern ENGINE_API float	particles_time;

struct V_DROP
{
	Fvector		P;
	Fvector		N;
	u32			Color;
	Fvector2	TC;
};

dxRainRender::dxRainRender()
{
	IReader* F = FS.r_open("$game_meshes$","dm\\rain.dm"); 
	VERIFY3(F,"Can't open file.","dm\\rain.dm");

	DM_Drop	= ::RImplementation.model_CreateDM(F);

	//
	SH_Rain.create("effects\\rain", "fx\\fx_rain");
	hGeom_Rain.create	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1, RCache.Vertex.Buffer(), RCache.QuadIB);
	hGeom_Drops.create	(FVF::F_LIT, RCache.Vertex.Buffer(), RCache.Index.Buffer());
	
	FS.r_close(F);
}

dxRainRender::~dxRainRender()
{
	::RImplementation.model_Delete(DM_Drop);
}

void dxRainRender::Copy(IRainRender &_in)
{
	*this = *(dxRainRender*)&_in;
}

#include "../../xrEngine/iGame_persistent.h"
void dxRainRender::Render(CEffectRain &owner)
{
	float factor = Environment().CurrentEnv->rain_density;
	if (factor < EPS_L)
		return;

  	u32 desired_items			= iFloor	(0.5f*(1.f+factor)*float(max_desired_items));
	// visual
	float		factor_visual	= factor/2.f+.5f;
	Fvector3	f_rain_color	= Environment().CurrentEnv->rain_color;
	u32			u_rain_color	= color_rgba_f(f_rain_color.x,f_rain_color.y,f_rain_color.z,factor_visual);

	// born _new_ if needed
	float b_radius_wrap_sqr = _sqr((source_radius + 0.5f));
	if (owner.items.size() < desired_items)
	{
		while (owner.items.size() < desired_items)
		{
			CEffectRain::Item one;
			owner.Born(one, source_radius);
			owner.items.push_back(one);
		}
	}

	// Build source plane
	Fplane src_plane;
	Fvector norm	= {0.f, -1.f, 0.f};
	Fvector upper; 	upper.set(Device.vCameraPosition.x, Device.vCameraPosition.y + source_offset, Device.vCameraPosition.z);
	src_plane.build(upper,norm);

	// Perform update
	u32 vOffset;
	V_DROP* verts = (V_DROP*)RCache.Vertex.Lock(desired_items * 4, hGeom_Rain->vb_stride, vOffset);
	V_DROP* start = verts;
	const Fvector&	vEye	= Device.vCameraPosition;

	for (CEffectRain::Item &item : owner.items)
	{
		// Physics and time control
		if (item.dwTime_Hit < Device.dwTimeGlobal)
			owner.Hit(item);

		if (item.dwTime_Life < Device.dwTimeGlobal)
			owner.Born(item, source_radius);

		item.P.mad(item.D, item.fSpeed * Device.fTimeDelta);

		Fvector	wdir; wdir.set(item.P.x - vEye.x, 0.0f, item.P.z - vEye.z);
		float wlen = wdir.square_magnitude();
		if (wlen > b_radius_wrap_sqr)
		{
			wlen = _sqrt(wlen);
			if ((item.P.y - vEye.y) < sink_offset)
			{
				// Need born
				item.Invalidate();
			}
			else
			{
				Fvector inv_dir, src_p;
				inv_dir.invert(item.D);
				wdir.div(wlen);
				item.P.mad(item.P, wdir, -(wlen + source_radius));
				if (src_plane.intersectRayPoint(item.P, inv_dir, src_p))
				{
					float dist_sqr = item.P.distance_to_sqr(src_p);
					float height = max_distance;
					u16 targetMaterial = u16(-1);
					if (owner.RayPick(src_p, item.D, height, targetMaterial, collide::rqtBoth))
					{
						if (_sqr(height) <= dist_sqr)
						{
							item.Invalidate(); // need born
						}
						else
						{
							owner.RenewItem(item, height - _sqrt(dist_sqr), targetMaterial, true); // fly to point
						}
					}
					else
					{
						owner.RenewItem(item, max_distance - _sqrt(dist_sqr), targetMaterial, false); // fly ...
					}
				}
				else
				{
					// need born
					item.Invalidate();
				}
			}
		}

		// Build line
		Fvector&	pos_head	= item.P;
		Fvector		pos_trail;	pos_trail.mad(pos_head, item.D, -drop_length * factor_visual);

		// Culling
		Fvector sC,lineD;	float sR; 
		sC.sub			(pos_head,pos_trail);
		lineD.normalize	(sC);
		sC.mul			(.5f);
		sR				= sC.magnitude();
		sC.add			(pos_trail);
		if (!::Render->ViewBase.testSphere_dirty(sC, sR))
			continue;

		static Fvector2 UV[2][4]=
		{
			{{0,1},{0,0},{1,1},{1,0}},
			{{1,0},{1,1},{0,0},{0,1}}
		};

		// Everything OK - build vertices
		Fvector	lineTop, camDir;
		camDir.sub(sC, vEye);
		camDir.normalize();
		lineTop.crossproduct(camDir, lineD);
		float w = drop_width;
		u32 s	= item.uv_set;

		Fvector P[4], N;
		// Calculate vertices positions
		P[0].mad(pos_trail, lineTop, -w);
		P[1].mad(pos_trail, lineTop,  w);
		P[2].mad(pos_head,  lineTop, -w);
		P[3].mad(pos_head,  lineTop,  w);
		
		// It's a plane so there is no need to calculate all vertex normals
		// since they all are pointing the same direction
		N.mknormal(P[0], P[1], P[2]);

		// Fill VB
		for (int i = 0; i < 4; ++i, ++verts)
		{
			verts->P		= P[i];
			verts->N		= N;
			verts->Color	= u_rain_color;
			verts->TC		= { UV[s][i].x, UV[s][i].y };
		}
	}
	u32 vCount = (u32)(verts - start);
	RCache.Vertex.Unlock(vCount, hGeom_Rain->vb_stride);

	// Render if needed
	if (vCount)
	{
		CEnvDescriptorMixer& envDesc = *Environment().CurrentEnv;

		Fvector4 colorAmb = { envDesc.ambient.x, envDesc.ambient.y, envDesc.ambient.z, 0.0f };
		colorAmb.mul(ps_r_sun_lumscale_amb * 2.0f);
		colorAmb.add(EPS);

		Fvector4 colorEnv = { envDesc.hemi_color.x, envDesc.hemi_color.y, envDesc.hemi_color.z, envDesc.weight };
		colorEnv.mul(ps_r_sun_lumscale_hemi * 4.0f);
		colorEnv.add(EPS);

		Fvector4 colorSun = { envDesc.sun_color.x, envDesc.sun_color.y, envDesc.sun_color.z, 0.0f };
		colorSun.mul(ps_r_sun_lumscale);
		colorSun.add(EPS);

		RCache.set_CullMode		(CULL_NONE);
		RCache.set_xform_world	(Fidentity);
		RCache.set_Shader		(SH_Rain);
		RCache.set_c			("L_ambient",		colorAmb);
		RCache.set_c			("L_hemi_color",	colorEnv);
		RCache.set_c			("L_sun_color",		colorSun);
		RCache.set_Geometry		(hGeom_Rain);
		RCache.Render			(D3DPT_TRIANGLELIST, vOffset, 0, vCount, 0, vCount / 2);
		RCache.set_CullMode		(CULL_CCW);
	}

	// Particles
	CEffectRain::Particle* pCurrParticle = owner.particle_active;
	if (!pCurrParticle)
		return;

	float dt = Device.fTimeDelta;
	_IndexStream& _IS = RCache.Index;
	RCache.set_Shader(DM_Drop->shader);

	Fmatrix mXform, mScale;
	int pcount = 0;
	u32 v_offset, i_offset;
	u32 vCount_Lock = particles_cache * DM_Drop->number_vertices;
	u32 iCount_Lock = particles_cache * DM_Drop->number_indices;
	IRender_DetailModel::fvfVertexOut* v_ptr = (IRender_DetailModel::fvfVertexOut*)RCache.Vertex.Lock(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
	u16* i_ptr = _IS.Lock(iCount_Lock, i_offset);
	while (pCurrParticle)
	{
		CEffectRain::Particle* pNextParticle = pCurrParticle->next;

		// Update
		pCurrParticle->time -= dt;
		if (pCurrParticle->time < 0.0f)
		{
			owner.ParticleFree(pCurrParticle);
			pCurrParticle = pNextParticle;
			continue;
		}

		// Render
		if (::Render->ViewBase.testSphere_dirty(pCurrParticle->bounds.P, pCurrParticle->bounds.R))
		{
			// Build matrix
			float scale = pCurrParticle->time / particles_time;
			mScale.scale(scale, scale, scale);
			mXform.mul_43(pCurrParticle->mXForm, mScale);

			// XForm verts
			DM_Drop->transfer(mXform, v_ptr, u_rain_color, i_ptr, pcount*DM_Drop->number_vertices);
			v_ptr += DM_Drop->number_vertices;
			i_ptr += DM_Drop->number_indices;
			pcount++;

			if (pcount >= particles_cache)
			{
				// flush
				u32	dwNumPrimitives		= iCount_Lock / 3;
				RCache.Vertex.Unlock	(vCount_Lock, hGeom_Drops->vb_stride);
				_IS.Unlock				(iCount_Lock);
				RCache.set_Geometry		(hGeom_Drops);
				RCache.Render			(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, dwNumPrimitives);

				v_ptr = (IRender_DetailModel::fvfVertexOut*)RCache.Vertex.Lock(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
				i_ptr = _IS.Lock(iCount_Lock, i_offset);

				pcount = 0;
			}
		}
		pCurrParticle = pNextParticle;
	}

	// Flush if needed
	vCount_Lock						= pcount*DM_Drop->number_vertices;
	iCount_Lock						= pcount*DM_Drop->number_indices;
	u32	dwNumPrimitives				= iCount_Lock / 3;
	RCache.Vertex.Unlock			(vCount_Lock, hGeom_Drops->vb_stride);
	_IS.Unlock						(iCount_Lock);
	if (pcount)
	{
		RCache.set_Geometry(hGeom_Drops);
		RCache.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, dwNumPrimitives);
	}
}

const Fsphere& dxRainRender::GetDropBounds() const
{
	return DM_Drop->bv_sphere;
}
