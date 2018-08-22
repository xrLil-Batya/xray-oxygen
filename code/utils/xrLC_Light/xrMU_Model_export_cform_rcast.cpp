#include "stdafx.h"
#include "xrMU_Model.h"
#include "xrMU_Model_Reference.h"

#include "../../xrcdb/xrcdb.h"
#include "../shader_xrlc.h"
void xrMU_Model::export_cform_rcast	(CDB::CollectorPacked& CL, Fmatrix& xform)
{
	for		(_face* face : m_faces)	face->flags.bProcessed = false;

	v_faces adjacent;	adjacent.reserve(6*2*3);

	for (_face*	F : m_faces)
	{
		const Shader_xrLC&	SH		= F->Shader();
		if (!SH.flags.bLIGHT_CastShadow)		continue;

		// Collect
		adjacent.clear	();
		for (_vertex* V : F->v)
		{
			for (_face* adj : V->m_adjacents)
				adjacent.push_back(adj);
		}

		// Unique
		std::sort		(adjacent.begin(),adjacent.end());
		adjacent.erase	(std::unique(adjacent.begin(),adjacent.end()),adjacent.end());
		bool bAlready = false;
		for (_face*	Test : adjacent)
		{
			if (Test==F)				continue;
			if (!Test->flags.bProcessed)continue;
			if (F->isEqual(*Test))
			{
				bAlready = true;
				break;
			}
		}

		//
		if (!bAlready) 
		{
			F->flags.bProcessed		= true;
			Fvector					P[3];
			xform.transform_tiny	(P[0],F->v[0]->P);
			xform.transform_tiny	(P[1],F->v[1]->P);
			xform.transform_tiny	(P[2],F->v[2]->P);
			CL.add_face_D			(P[0],P[1],P[2],*((size_t*)&F), F->sm_group );//
		}
	}
}

