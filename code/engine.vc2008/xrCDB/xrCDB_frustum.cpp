//////////////////////////////////////////////////////////
// Desc     : Furstrum Collider
// Author   : ForserX
// Original : GSC
//////////////////////////////////////////////////////////
// Oxygen Engine (2016-2019)
//////////////////////////////////////////////////////////

#include "stdafx.h"

#include "xrCDB.h"
#include "frustum.h"
#include "xrCDB_Model.h"

using namespace CDB;
using namespace Opcode;

class CFrustumCollider
{
public:
	COLLIDER* dest;
	TRI* tris;
	Fvector* verts;
	const CFrustum* F;
	bool bClass3; bool bFirst;

	CFrustumCollider(COLLIDER* CL, Fvector* V, TRI* T, const CFrustum* _F, bool bClass, bool bFrst) : bClass3(bClass), bFirst(bFrst)
	{
		dest = CL;
		tris = T;
		verts = V;
		F = _F;
	}

	IC EFC_Visible Box(const Fvector& C, const Fvector& E, u32& mask)
	{
		Fvector mM;
		mM.sub(C, E);
		//mM[1].add(C, E);

		return F->testAABB(&mM.x, mask);
	}

	void Prim(DWORD prim)
	{
#define SetByVerts(val) \
			val[0] = verts[tris[prim].verts[0]];\
			val[1] = verts[tris[prim].verts[1]];\
			val[2] = verts[tris[prim].verts[2]];\

		if (bClass3)
		{
			sPoly src, dst;
			src.resize(3);
			SetByVerts(src);

			if (F->ClipPoly(src, dst))
			{
				RESULT& R = dest->r_add();
				R.id = prim;
				SetByVerts(R.verts);
				R.dummy = tris[prim].dummy;
			}
		}
		else
		{
			RESULT& R = dest->r_add();
			R.id = prim;
			SetByVerts(R.verts);
			R.dummy = tris[prim].dummy;
		}
	}

	void Stab(const AABBNoLeafNode* node, u32 mask)
	{
		// Actual frustum/aabb test
		EFC_Visible	result = Box((Fvector&)node->mAABB.mCenter, (Fvector&)node->mAABB.mExtents, mask);
		if (fcvNone == result)	return;

		// 1st chield
		if (node->HasPosLeaf())	Prim((DWORD)node->GetPosPrimitive());
		else					Stab(node->GetPos(), mask);

		// Early exit for "only first"
		if (bFirst)
			if (dest->r_count()) return;

		// 2nd chield
		if (node->HasNegLeaf())	Prim((DWORD)node->GetNegPrimitive());
		else					Stab(node->GetNeg(), mask);
	}
};

void COLLIDER::frustum_query(const MODEL* m_def, const CFrustum& F)
{
	m_def->syncronize();

	// Get nodes
	const AABBNoLeafNode* pNodes = m_def->tree->GetTree()->GetNodes();
	const DWORD				mask = F.getMask();
	r_clear();

	// Binary dispatcher
	CFrustumCollider BC(this, m_def->verts, m_def->tris, &F, frustum_mode & OPT_FULL_TEST, frustum_mode & OPT_ONLYFIRST);
	BC.Stab(pNodes, mask);
}
