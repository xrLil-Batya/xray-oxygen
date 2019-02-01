#include "stdafx.h"
#include "../../xrEngine/DirectXMathExternal.h"

// true if intersects near plane
BOOL CRenderTarget::enable_scissor(light* L)
{
	// Near plane intersection
	BOOL	near_intersect				= FALSE;
	{
		Fmatrix& M = Device.mFullTransform;
		Fvector4 plane;
		plane.x = -(M._14 + M._13);
		plane.y = -(M._24 + M._23);
		plane.z = -(M._34 + M._33);
		plane.w = -(M._44 + M._43);
		float denom = -1.0f / _sqrt(_sqr(plane.x) + _sqr(plane.y) + _sqr(plane.z));
		plane.mul(denom);
		Fplane	P;	P.n.set(plane.x, plane.y, plane.z); P.d = plane.w;
		float	p_dist = P.classify(L->spatial.sphere.P) - L->spatial.sphere.R;
		near_intersect = (p_dist <= 0);
	}
#ifdef DEBUG
	
	Fsphere		S;	S.set	(L->spatial.sphere.P,L->spatial.sphere.R);
	dbg_spheres.push_back	(std::make_pair(S,L->color));
	
#endif

	return		near_intersect;
}
