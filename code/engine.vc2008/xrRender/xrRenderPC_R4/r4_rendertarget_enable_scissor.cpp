#include "stdafx.h"
#include "../../xrEngine/DirectXMathExternal.h"

// true if intersects near plane
BOOL CRenderTarget::enable_scissor(light* L)
{
	// Near plane intersection
	BOOL	near_intersect				= FALSE;
	{
		Fvector4 plane;
		plane.x							= -(Device.mFullTransform.x[3] + Device.mFullTransform.x[2]);
		plane.y							= -(Device.mFullTransform.y[3] + Device.mFullTransform.y[2]);
		plane.z							= -(Device.mFullTransform.z[3] + Device.mFullTransform.z[2]);
		plane.w							= -(Device.mFullTransform.w[3] + Device.mFullTransform.w[2]);
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

	return		near_intersect;
}
