// WallmarksEngine.cpp: implementation of the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WallmarksEngine.h"

#include "../../xrEngine/xr_object.h"
#include "../../xrEngine/x_ray.h"
#include "../../xrEngine/GameFont.h"
#include "SkeletonCustom.h"

namespace WallmarksEngine
{
	struct wm_slot
	{
		using StaticWMVec = CWallmarksEngine::StaticWMVec;

		ref_shader		shader;
		StaticWMVec		static_items;

		xr_vector< intrusive_ptr<CSkeletonWallmark>> skeleton_items;

		wm_slot(ref_shader sh) : shader(sh)
		{
			static_items.reserve(256); 
			skeleton_items.reserve(256);
		}
	};
}

// #include "xr_effsun.h"

constexpr float W_DIST_FADE		= 15.f;
constexpr float	W_DIST_FADE_SQR	= W_DIST_FADE*W_DIST_FADE;
constexpr float I_DIST_FADE_SQR	= 1.f/W_DIST_FADE_SQR;
constexpr u32	MAX_TRIS		= 1024;

IC bool operator == (const CWallmarksEngine::wm_slot* slot, const ref_shader& shader){return slot->shader==shader;}
CWallmarksEngine::wm_slot* CWallmarksEngine::FindSlot(ref_shader shader)
{
	auto it = std::find(marks.begin(), marks.end(), shader);
	return (it != marks.end()) ? *it : nullptr;
}

CWallmarksEngine::wm_slot* CWallmarksEngine::AppendSlot	(ref_shader shader)
{
	marks.push_back(xr_new<wm_slot>(shader));
	return marks.back();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWallmarksEngine::CWallmarksEngine()
{
	static_pool.reserve(256);
	marks.reserve(256);
	hGeom.create(FVF::F_LIT, RCache.Vertex.Buffer(), nullptr);
}

CWallmarksEngine::~CWallmarksEngine()
{
	clear();
	hGeom.destroy();
}

void CWallmarksEngine::clear()
{
	for (WallmarksEngine::wm_slot* mark : marks)
	{
		for (CWallmarksEngine::static_wallmark* static_item : mark->static_items)
			static_wm_destroy(static_item);

		xr_delete(mark);
	}
	marks.clear();

	for (CWallmarksEngine::static_wallmark* pStaticWallmark : static_pool)
		xr_delete(pStaticWallmark);
	static_pool.clear();
}

// allocate
CWallmarksEngine::static_wallmark*	CWallmarksEngine::static_wm_allocate()
{
	static_wallmark* pWMarks = nullptr;

	if (!static_pool.empty()) 
	{ 
		pWMarks = static_pool.back();
		static_pool.pop_back();
	}
	else pWMarks = new static_wallmark();

	pWMarks->ttl = ps_r_WallmarkTTL;
	pWMarks->verts.clear();
	return pWMarks;
}

// destroy
void CWallmarksEngine::static_wm_destroy(CWallmarksEngine::static_wallmark* W)
{
	static_pool.push_back(W);
}

// render
void CWallmarksEngine::static_wm_render(CWallmarksEngine::static_wallmark*	W, FVF::LIT* &V)
{
	float		a = 1 - (W->ttl / ps_r_WallmarkTTL);
	int			aC = iFloor(a * 255.f);	clamp(aC, 0, 255);
	u32			C = color_rgba(128, 128, 128, aC);
	for (const FVF::LIT& elem : W->verts) {
		V->p.set(elem.p);
		V->color = C;
		V->t.set(elem.t);
	}
}

//--------------------------------------------------------------------------------
void CWallmarksEngine::RecurseTri(u32 t, Fmatrix &mView, CWallmarksEngine::static_wallmark	&W)
{
	CDB::TRI*	T			= sml_collector.getT()+t;
	if (T->dummy)			return;
	T->dummy				= 0xffffffff;
	
	// Some vars
	u32*		v_ids		= T->verts;
	Fvector*	v_data		= sml_collector.getV();
	sml_poly_src.clear		();
	sml_poly_src.push_back	(v_data[v_ids[0]]);
	sml_poly_src.push_back	(v_data[v_ids[1]]);
	sml_poly_src.push_back	(v_data[v_ids[2]]);
	sml_poly_dest.clear		();
	
	sPoly* P = sml_clipper.ClipPoly	(sml_poly_src, sml_poly_dest);
	
	//. todo
	// uv_gen = mView * []
	// UV = pos*uv_gen

	if (P) {
		// Create vertices and triangulate poly (tri-fan style triangulation)
		FVF::LIT			V0,V1,V2;
		Fvector				UV;

		mView.transform_tiny(UV, (*P)[0]);
		V0.set				((*P)[0],0,(1+UV.x)*.5f,(1-UV.y)*.5f);
		mView.transform_tiny(UV, (*P)[1]);
		V1.set				((*P)[1],0,(1+UV.x)*.5f,(1-UV.y)*.5f);

		for (u32 i=2; i<P->size(); i++)
		{
			mView.transform_tiny(UV, (*P)[i]);
			V2.set				((*P)[i],0,(1+UV.x)*.5f,(1-UV.y)*.5f);
			W.verts.push_back	(V0);
			W.verts.push_back	(V1);
			W.verts.push_back	(V2);
			V1					= V2;
		}
		
		// recurse
		for (int i=0; i<3; i++)
		{
			u32 adj					= sml_adjacency[3*t+i];
			if (0xffffffff==adj)	continue;
			CDB::TRI*	SML			= sml_collector.getT() + adj;
			v_ids					= SML->verts;

			Fvector test_normal;
			test_normal.mknormal	(v_data[v_ids[0]],v_data[v_ids[1]],v_data[v_ids[2]]);
			float cosa				= test_normal.dotproduct(sml_normal);
			if (cosa<0.034899f)		continue;	// cos(88)
			RecurseTri				(adj,mView,W);
		}
	}
}

void CWallmarksEngine::BuildMatrix	(Fmatrix &mView, float invsz, const Fvector& from)
{
	// build projection
	Fmatrix				mScale;
    Fvector				at,up,right,y;
	at.sub				(from,sml_normal);
	y.set				(0,1,0);
	if (_abs(sml_normal.y)>.99f) y.set(1,0,0);
	right.crossproduct	(y,sml_normal);
	up.crossproduct		(sml_normal,right);
	mView.build_camera	(from,at,up);
	mScale.scale		(invsz,invsz,invsz);
	mView.mulA_43		(mScale);
}

void CWallmarksEngine::AddWallmark_internal	(CDB::TRI* pTri, const Fvector* pVerts, const Fvector &contact_point, ref_shader hShader, float sz)
{
	// query for polygons in bounding box
	// calculate adjacency
	{
		Fbox				bb_query;
		Fvector				bbc,bbd;
		bb_query.set		(contact_point,contact_point);
		bb_query.grow		(sz*2.5f);
		bb_query.get_CD		(bbc,bbd);
		xrc.box_options		(CDB::OPT_FULL_TEST);
		xrc.box_query		(g_pGameLevel->ObjectSpace.GetStaticModel(),bbc,bbd);
		u32	triCount		= (u32)xrc.r_count	();
		if (0==triCount)	
			return;

		CDB::TRI* tris		= g_pGameLevel->ObjectSpace.GetStaticTris();
		sml_collector.clear	();
		sml_collector.add_face_packed_D	(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]],0);
		for (u32 t=0; t<triCount; t++)	
		{
			CDB::RESULT& sectorTris = xrc.r_getElement(t);
			CDB::TRI*	T	= tris + sectorTris.id;
			if (T==pTri)	continue;
			sml_collector.add_face_packed_D		(pVerts[T->verts[0]],pVerts[T->verts[1]],pVerts[T->verts[2]],0);
		}
		sml_collector.calc_adjacency	(sml_adjacency);
	}

	// calc face normal
	Fvector	N;
	N.mknormal			(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
	sml_normal.set		(N);

	// build 3D ortho-frustum
	Fmatrix				mView,mRot;
	BuildMatrix			(mView,1/sz,contact_point);
	mRot.rotateZ		(::Random.randF(deg2rad(-20.f),deg2rad(20.f)));
	mView.mulA_43		(mRot);
	sml_clipper.CreateFromMatrix	(mView,FRUSTUM_P_LRTB);

	// create wallmark
	static_wallmark* W	= static_wm_allocate();
	RecurseTri			(0, mView, *W);

	// calc sphere
	if (W->verts.size()<3) 
	{ 
		static_wm_destroy(W); 
		return; 
	}else 
	{
		Fbox bb;	bb.invalidate();

		for (FVF::LIT& elem : W->verts)
		{
			bb.modify(elem.p);
		}
		bb.getsphere					(W->bounds.P, W->bounds.R);
	}

	{
		// search if similar wallmark exists
		wm_slot* slot			= FindSlot	(hShader);
		if (slot)
		{
            auto it	=	slot->static_items.begin	();
            auto end	=	slot->static_items.end	();
			for (; it!=end; it++)	
			{
				static_wallmark* wm		=	*it;
				if (wm->bounds.P.similar(W->bounds.P,0.02f))
				{ // replace
					static_wm_destroy	(wm);
					*it					=	W;
					return;
				}
			}
		} else {
			slot		= AppendSlot(hShader);
		}

		// no similar - register _new_
		slot->static_items.push_back(W);
	}
}

void CWallmarksEngine::AddStaticWallmark	(CDB::TRI* pTri, const Fvector* pVerts, const Fvector &contact_point, ref_shader hShader, float sz)
{
	// optimization cheat: don't allow wallmarks more than 100 m from viewer/actor
	if (contact_point.distance_to_sqr(Device.vCameraPosition) > _sqr(100.f))	
		return;

	// Physics may add wallmarks in parallel with rendering
	xrCriticalSectionGuard guard(lock);
	AddWallmark_internal	(pTri,pVerts,contact_point,hShader,sz);
}

void CWallmarksEngine::AddSkeletonWallmark(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)
{
	if (::RImplementation.phase != CRender::PHASE_NORMAL)				return;
	// optimization cheat: don't allow wallmarks more than 50 m from viewer/actor
	if (xf->c.distance_to_sqr(Device.vCameraPosition) > _sqr(50.f))				return;

	VERIFY(obj&&xf && (size > EPS_L));
	xrCriticalSectionGuard guard(lock);
	obj->AddWallmark(xf, start, dir, sh, size);
}

void CWallmarksEngine::AddSkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm)
{
	if(::RImplementation.phase != CRender::PHASE_NORMAL) return;

	if (!::RImplementation.val_bHUD)
	{
		xrCriticalSectionGuard guard(lock);
		// search if similar wallmark exists
		wm_slot* slot		= FindSlot	(wm->Shader());
		if (nullptr==slot) slot	= AppendSlot(wm->Shader());
		// no similar - register _new_
		slot->skeleton_items.push_back(wm);
		wm->used_in_render	= Device.dwFrame;
	}
}

extern float r_ssaDISCARD;
ICF void BeginStream(ref_geom hGeom, u32& w_offset, FVF::LIT*& w_verts, FVF::LIT*& w_start)
{
	w_offset				= 0;
	w_verts					= (FVF::LIT*)RCache.Vertex.Lock	(MAX_TRIS*3,hGeom->vb_stride,w_offset);
	w_start					= w_verts;
}

ICF void FlushStream(ref_geom hGeom, ref_shader shader, u32& w_offset, FVF::LIT*& w_verts, FVF::LIT*& w_start, BOOL bSuppressCull)
{
	u32 w_count					= u32(w_verts-w_start);
	RCache.Vertex.Unlock		(w_count,hGeom->vb_stride);
	if (w_count)			
	{
		RCache.set_Shader		(shader);
		RCache.set_Geometry		(hGeom);
		if (bSuppressCull)		RCache.set_CullMode (D3D11_CULL_NONE);
		RCache.Render			(D3DPT_TRIANGLELIST,w_offset,w_count/3);
		if (bSuppressCull)		RCache.set_CullMode	(D3D11_CULL_BACK);
		Device.Statistic->RenderDUMP_WMT_Count += w_count/3;
	}
}

void CWallmarksEngine::Render()
{
	// Projection and xform
	Fmatrix WallmarksProject = Device.mProject;

	WallmarksProject._43			-= ps_r_WallmarkSHIFT;
	RCache.set_xform_world		(Fidentity);
	RCache.set_xform_project	(WallmarksProject);

	Fmatrix mSavedView = Device.mView;
	Fvector	mViewPos			;
	mViewPos.mad(Device.vCameraPosition, Device.vCameraDirection, ps_r_WallmarkSHIFT_V);
	Device.mView.build_camera_dir(mViewPos, Device.vCameraDirection, Device.vCameraTop);
	RCache.set_xform_view		(Device.mView);

	Device.Statistic->RenderDUMP_WM.Begin	();
	Device.Statistic->RenderDUMP_WMS_Count	= 0;
	Device.Statistic->RenderDUMP_WMD_Count	= 0;
	Device.Statistic->RenderDUMP_WMT_Count	= 0;

	float	ssaCLIP				= r_ssaDISCARD/4;

	lock.Enter();			// Physics may add wallmarks in parallel with rendering

	for (wm_slot* slot : marks) 
	{
		u32			w_offset;
		FVF::LIT	*w_verts, *w_start;
		BeginStream(hGeom, w_offset, w_verts, w_start);
		// static wallmarks
		for (auto w_it = slot->static_items.begin(); w_it != slot->static_items.end(); )
		{
			static_wallmark* W = *w_it;
			if (RImplementation.ViewBase.testSphere_dirty(W->bounds.P, W->bounds.R)) {
				Device.Statistic->RenderDUMP_WMS_Count++;
				float dst = Device.vCameraPosition.distance_to_sqr(W->bounds.P);
				float ssa = W->bounds.R * W->bounds.R / dst;
				if (ssa >= ssaCLIP) 
				{
					u32 w_count = u32(w_verts - w_start);
					if ((w_count + W->verts.size()) >= (MAX_TRIS * 3)) {
						FlushStream(hGeom, slot->shader, w_offset, w_verts, w_start, FALSE);
						BeginStream(hGeom, w_offset, w_verts, w_start);
					}
					static_wm_render(W, w_verts);
				}
				W->ttl -= 0.1f*Device.fTimeDelta;	// visible wallmarks fade much slower
			}
			else 
			{
				W->ttl -= Device.fTimeDelta;
			}

			if (W->ttl <= EPS) 
			{
				static_wm_destroy(W);
				*w_it = slot->static_items.back();
				slot->static_items.pop_back();
			}
			else 
			{
				w_it++;
			}
		}
		// Flush stream
		FlushStream(hGeom, slot->shader, w_offset, w_verts, w_start, FALSE);	//. remove line if !(suppress cull needed)
		BeginStream(hGeom, w_offset, w_verts, w_start);

		// dynamic wallmarks
		for (intrusive_ptr<CSkeletonWallmark> pSkeletonWallmark: slot->skeleton_items)
		{
			if (!pSkeletonWallmark)
				continue;

			Device.Statistic->RenderDUMP_WMD_Count++;
			u32 w_count = u32(w_verts - w_start);
			if ((w_count + pSkeletonWallmark->VCount()) >= (MAX_TRIS * 3))
			{
				FlushStream(hGeom, slot->shader, w_offset, w_verts, w_start, TRUE);
				BeginStream(hGeom, w_offset, w_verts, w_start);
			}

			FVF::LIT *w_save = w_verts;
			try
			{
				pSkeletonWallmark->Parent()->RenderWallmark(pSkeletonWallmark, w_verts);
			}
			catch (...)
			{
				Msg("! Failed to render dynamic wallmark");
				w_verts = w_save;
			}
			pSkeletonWallmark->used_in_render = u32(-1);
		}
		slot->skeleton_items.clear();
		// Flush stream
		FlushStream(hGeom, slot->shader, w_offset, w_verts, w_start, TRUE);
	}

	lock.Leave();				// Physics may add wallmarks in parallel with rendering

	// Level-wmarks
	RImplementation.r_dsgraph_render_wmarks	();
	Device.Statistic->RenderDUMP_WM.End		();

	// Projection
	Device.mView				= mSavedView;
	RCache.set_xform_view		(Device.mView);
	RCache.set_xform_project	(Device.mProject);
}
