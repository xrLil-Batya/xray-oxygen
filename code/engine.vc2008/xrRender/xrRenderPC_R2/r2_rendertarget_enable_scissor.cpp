#include "stdafx.h"

void CRenderTarget::enable_dbt_bounds		(light* L)
{
	if (!RImplementation.o.nvdbt)					return;
	if (!ps_r_flags.test(R_FLAG_USE_NVDBT))		return;

	u32	mask		= 0xffffffff;
	EFC_Visible vis	= RImplementation.ViewBase.testSphere(L->spatial.sphere.P,L->spatial.sphere.R*1.01f,mask);
	if (vis!=fcvFully)								return;

	// xform BB
	Fbox	BB;
	Fvector	rr; rr.set(L->spatial.sphere.R,L->spatial.sphere.R,L->spatial.sphere.R);
	BB.setb	(L->spatial.sphere.P, rr);

	Fbox	bbp; bbp.invalidate();
	for (u32 i=0; i<8; i++)		{
		Fvector		pt;
		BB.getpoint	(i,pt);
		CastToGSCMatrix(Device.mFullTransform).transform	(pt);
		bbp.modify	(pt);
	}
	u_DBT_enable	(bbp.min.z,bbp.max.z);
}

// nv-DBT
BOOL	CRenderTarget::u_DBT_enable	(float zMin, float zMax)
{
	if (!RImplementation.o.nvdbt)					return	FALSE;
	if (!ps_r_flags.test(R_FLAG_USE_NVDBT))		return	FALSE;

	// enable cheat
	HW.pDevice->SetRenderState(D3DRS_ADAPTIVETESS_X,MAKEFOURCC('N','V','D','B'));
	HW.pDevice->SetRenderState(D3DRS_ADAPTIVETESS_Z,*(DWORD*)&zMin);
	HW.pDevice->SetRenderState(D3DRS_ADAPTIVETESS_W,*(DWORD*)&zMax); 

	return TRUE;
}

void	CRenderTarget::u_DBT_disable	()
{
	if (RImplementation.o.nvdbt && ps_r_flags.test(R_FLAG_USE_NVDBT))	
		HW.pDevice->SetRenderState(D3DRS_ADAPTIVETESS_X,0);
}

BOOL CRenderTarget::enable_scissor		(light* L)		// true if intersects near plane
{
	// Near plane intersection
	BOOL	near_intersect				= FALSE;
	{
		Fmatrix& M						= CastToGSCMatrix(Device.mFullTransform);
		Fvector4 plane;
		plane.x							= -(M._14 + M._13);
		plane.y							= -(M._24 + M._23);
		plane.z							= -(M._34 + M._33);
		plane.w							= -(M._44 + M._43);
		float denom						= -1.0f / _sqrt(_sqr(plane.x)+_sqr(plane.y)+_sqr(plane.z));
		plane.mul						(denom);
		Fplane	P;	P.n.set(plane.x,plane.y,plane.z); P.d = plane.w;
		float	p_dist					= P.classify	(L->spatial.sphere.P) - L->spatial.sphere.R;
		near_intersect					= (p_dist<=0);
	}
#ifdef DEBUG
	
	Fsphere		S;	S.set	(L->spatial.sphere.P,L->spatial.sphere.R);
	dbg_spheres.push_back	(std::make_pair(S,L->color));
	
#endif

	// Scissor
	//. disable scissor because some bugs prevent it to work through multi-portals
	//. if (!HW.Caps.bScissor)	return		near_intersect;
	return		near_intersect;
}