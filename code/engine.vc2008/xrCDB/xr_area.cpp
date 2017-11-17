#include "stdafx.h"
//#include "igame_level.h"

#include "xr_area.h"
#include "../xrengine/xr_object.h"
#include "../xrengine/xrLevel.h"
#include "../xrengine/xr_collide_form.h"
//#include "../xrsound/sound.h"
//#include "x_ray.h"
//#include "GameFont.h"


using namespace	collide;



//----------------------------------------------------------------------
// Class	: CObjectSpace
// Purpose	: stores space slots
//----------------------------------------------------------------------
CObjectSpace::CObjectSpace() :
	xrc()
	, m_pRender(0)
{
	if (RenderFactory)
		m_pRender = CNEW(FactoryPtr<IObjectSpaceRender>)();

	//sh_debug.create				("debug\\wireframe","$nullptr");
	m_BoundingVolume.invalidate();
}
//----------------------------------------------------------------------
CObjectSpace::~CObjectSpace()
{
	//moved to ~IGameLevel
	//	Sound->set_geometry_occ		(nullptr);
	//	Sound->set_handler			(nullptr);
	//
#ifdef DEBUG
	//sh_debug.destroy			();
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
	xr_vector<ISpatial*>::iterator	it = q_spatial.begin();
	xr_vector<ISpatial*>::iterator	end = q_spatial.end();
	for (; it != end; it++) {
		CObject* O = (*it)->dcast_CObject();
		if (0 == O)				continue;
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
	return				GetNearest(q_nearest, O->spatial.sphere.P, range + O->spatial.sphere.R, O);
}

//----------------------------------------------------------------------


void CObjectSpace::Load(CDB::build_callback build_callback)
{
	Load("$level$", "level.cform", build_callback);
}

void CObjectSpace::Load(const char* path, const char* fname, CDB::build_callback build_callback)
{
	IReader *F = FS.r_open(path, fname);
	R_ASSERT(F);
	Load(F, build_callback);
}

void CObjectSpace::Load(IReader* F, CDB::build_callback build_callback)
{
	hdrCFORM realCform;
	F->r(&realCform, sizeof(hdrCFORM));
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//if (realCform.version != CFORM_CURRENT_VERSION)													///
	//{																									///
	//	hdrCFORM_4 oldCform;																			///
	//	F->r(&oldCform, sizeof(hdrCFORM_4));															///
	R_ASSERT2(realCform.version == CFORM_CURRENT_VERSION || realCform.version == 4, "Incorrect level.cform! xrOxygen supports ver.4 and ver.5."); ///
	//																									///
	//	realCform.aabb = oldCform.aabb;																	///
	//	realCform.facecount = oldCform.facecount;														///
	//	realCform.vertcount = oldCform.vertcount;														///
	//}																									///
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	Fvector* verts = (Fvector*)F->pointer();
	CDB::TRI* tris = (CDB::TRI*)(verts + realCform.vertcount);
	Create(verts, tris, realCform, build_callback);
	FS.r_close(F);
}

void CObjectSpace::Create(Fvector*	verts, CDB::TRI* tris, const hdrCFORM &H, CDB::build_callback build_callback)
{
	Static.build(verts, H.vertcount, tris, H.facecount, build_callback);
	m_BoundingVolume.set(H.aabb);
	g_SpatialSpace->initialize(m_BoundingVolume);
	g_SpatialSpacePhysic->initialize(m_BoundingVolume);
}

//----------------------------------------------------------------------
void CObjectSpace::dbgRender()
{
	(*m_pRender)->dbgRender();
}
