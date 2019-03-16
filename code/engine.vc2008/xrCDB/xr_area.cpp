#include "stdafx.h"
#include "xr_area.h"
#include "../xrengine/xr_object.h"
#include "../xrengine/xrLevel.h"
#include "../xrengine/xr_collide_form.h"

using namespace	collide;

//----------------------------------------------------------------------
// Class	: CObjectSpace
// Purpose	: stores space slots
//----------------------------------------------------------------------
CObjectSpace::CObjectSpace(): xrc(), m_pRender(nullptr)
{
	if (RenderFactory)
		m_pRender = CNEW(FactoryPtr<IObjectSpaceRender>)();

	m_BoundingVolume.invalidate();
}
//----------------------------------------------------------------------
CObjectSpace::~CObjectSpace()
{
#ifdef DEBUG
	CDELETE(m_pRender);
#endif
}
//----------------------------------------------------------------------
#pragma warning(disable: 4267)
//----------------------------------------------------------------------
int CObjectSpace::GetNearest(xr_vector<ISpatial*>& q_spatial, xr_vector<CObject*>&	q_nearest, const Fvector &point, float range, CObject* ignore_object)
{
	q_spatial.clear();
	// Query objects
	q_nearest.clear();
	Fsphere				Q;	Q.set(point, range);
	Fvector				B;	B.set(range, range, range);
	g_SpatialSpace->q_box(q_spatial, 0, STYPE_COLLIDEABLE, point, B);

	// Iterate
	for (ISpatial* it: q_spatial) {
		CObject* O = it->dcast_CObject();
		if (nullptr == O)				continue;
		if (O == ignore_object)	continue;
		Fsphere mS = { O->spatial.sphere.P, O->spatial.sphere.R };
		if (Q.intersect(mS))	q_nearest.push_back(O);
	}

	return q_nearest.size();
}

//----------------------------------------------------------------------
IC int	CObjectSpace::GetNearest(xr_vector<CObject*>&	q_nearest, const Fvector &point, float range, CObject* ignore_object)
{
	return(GetNearest(r_spatial, q_nearest, point, range, ignore_object));
}

//----------------------------------------------------------------------
IC int   CObjectSpace::GetNearest(xr_vector<CObject*>&	q_nearest, ICollisionForm* obj, float range)
{
	CObject*	O = obj->Owner();
	return GetNearest(q_nearest, O->spatial.sphere.P, range + O->spatial.sphere.R, O);
}

//----------------------------------------------------------------------
void CObjectSpace::Load(CDB::build_callback build_callback)
{
	IReader *F = FS.r_open("$level$", "level.cform");
	R_ASSERT2(F, "Level cform not found!");

	// Cache for cform
	u32 crc = crc32(F->pointer(), F->length());
	string_path LevelName;
	xr_strconcat(LevelName, FS.get_path("$level$")->m_Add, "cform.cache");

	IReader* pReaderCache = FS.r_open("$level_cache$", LevelName);

	hdrCFORM realCform;
	F->r(&realCform, sizeof(hdrCFORM));

	R_ASSERT2(realCform.version == CFORM_CURRENT_VERSION, "Incorrect level.cform! xrOxygen supports ver.4 and ver.5.");

	Fvector* verts = (Fvector*)F->pointer();
	CDB::TRI* tris = (CDB::TRI*)(verts + realCform.vertcount);

	if (pReaderCache && pReaderCache->length() > 4 && pReaderCache->r_u32() == crc)
	{
		Create(verts, tris, realCform, build_callback, pReaderCache);
	}
	else
	{
		IWriter* pWriterCache = FS.w_open("$level_cache$", LevelName);
		pWriterCache->w_u32(crc);
		Create(verts, tris, realCform, build_callback, pWriterCache);
	}

	FS.r_close(F);
}

void CObjectSpace::Create(Fvector* verts, CDB::TRI* tris, const hdrCFORM &H, CDB::build_callback build_callback, IReader* pFS)
{
	Static.build(verts, H.vertcount, tris, H.facecount, pFS, true, build_callback);
	m_BoundingVolume.set(H.aabb);
	g_SpatialSpace->initialize(m_BoundingVolume);
	g_SpatialSpacePhysic->initialize(m_BoundingVolume);
}

void CObjectSpace::Create(Fvector* verts, CDB::TRI* tris, const hdrCFORM &H, CDB::build_callback build_callback, IWriter* pFS)
{
	Static.build(verts, H.vertcount, tris, H.facecount, pFS, false, build_callback);
	m_BoundingVolume.set(H.aabb);
	g_SpatialSpace->initialize(m_BoundingVolume);
	g_SpatialSpacePhysic->initialize(m_BoundingVolume);
}

//----------------------------------------------------------------------
void CObjectSpace::dbgRender()
{
	(*m_pRender)->dbgRender();
}
