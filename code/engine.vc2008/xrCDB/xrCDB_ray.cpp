#include "stdafx.h"
#pragma hdrstop
#pragma warning(push)
#pragma warning(disable:4995)
#include <xmmintrin.h>
#pragma warning(pop)

#include "xrCDB.h"
#include "xrCDB_Model.h"

using namespace CDB;
using namespace Opcode;

// can you say "barebone"?
#ifndef _MM_ALIGN16
#define _MM_ALIGN16	__declspec(align(16))
#endif // _MM_ALIGN16

struct	_MM_ALIGN16		vec_t	
:	public Fvector3	
{ 
	float		pad; 
};
//static vec_t	vec_c	( float _x, float _y, float _z)	{ vec_t v; v.x=_x;v.y=_y;v.z=_z;v.pad=0; return v; }

struct _MM_ALIGN16		aabb_t	
{ 
	vec_t		min;
	vec_t		max;
};
struct _MM_ALIGN16		ray_t	
{
	vec_t		pos;
	vec_t		inv_dir;
	vec_t		fwd_dir;
};

#ifdef __AVX_USE__
/************************************************
*#VERTVER: AVX use the part of SSE, and some
*of AVX instructions
* SSE - xmm, AVX - ymm
* Size of register: AVX-256, SSE-128
*************************************************/

// load ymm
#define loadps(mem)			_mm256_load_ps((const float * const)(mem))
#define storeps(ss,mem)		_mm256_store_ps((float * const)(mem),(ss))
#define storess(ss,mem)		_mm_store_ss((float * const)(mem),(ss))
#define minps				_mm256_min_ps
#define minpssse			_mm_min_ps
#define maxps				_mm256_max_ps
#define maxpssse			_mm_max_ps
#define mulps				_mm256_mul_ps
#define subps				_mm256_sub_ps
#define rotatelps(ps)		_mm256_shuffle_ps((ps),(ps), 0x39)		// a,b,c,d -> b,c,d,a
#define rotatelpssse(ps)	_mm_shuffle_ps((ps),(ps), 0x39)	
// NO ANALOG IN AVX
#define muxhps(low,high)	_mm_movehl_ps((low),(high))		// low {a,b,c,d} | high {e,f,g,h} = {c,d,g,h}


// SSE types
#define storess(ss,mem)		_mm_store_ss((float * const)(mem),(ss))
#define minss				_mm_min_ss
#define maxss				_mm_max_ss
#define mm128_cast			_mm256_castps256_ps128
#define mm256_cast			_mm256_castps128_ps256

#else 

// SSE and SSE2
// turn those verbose intrinsics into something readable.
#define loadps(mem)			_mm_load_ps((const float * const)(mem))
#define storess(ss,mem)		_mm_store_ss((float * const)(mem),(ss))
#define minss				_mm_min_ss
#define maxss				_mm_max_ss
#define minps				_mm_min_ps
#define maxps				_mm_max_ps
#define mulps				_mm_mul_ps
#define subps				_mm_sub_ps
#define rotatelps(ps)		_mm_shuffle_ps((ps),(ps), 0x39)	
#define muxhps(low,high)	_mm_movehl_ps((low),(high))		

#endif

static const float flt_plus_inf = -logf(0);	// let's keep C and C++ compilers happy.
static const float _MM_ALIGN16
	ps_cst_plus_inf	[4]	=	{  flt_plus_inf,  flt_plus_inf,  flt_plus_inf,  flt_plus_inf },
	ps_cst_minus_inf[4]	=	{ -flt_plus_inf, -flt_plus_inf, -flt_plus_inf, -flt_plus_inf };


#ifdef __AVX_USE__
//#VERTVER: It's working. I'll guarantee it
////////////////////////////////////////////////
ICF bool isect_sse(const aabb_t &box, const ray_t &ray, float &dist)
{
	// you may already have those values hanging around somewhere
	const __m256
		plus_inf = loadps(ps_cst_plus_inf),
		minus_inf = loadps(ps_cst_minus_inf);

	// use whatever's apropriate to load.
	const __m256
		box_min = loadps(&box.min),
		box_max = loadps(&box.max),
		pos = loadps(&ray.pos),
		inv_dir = loadps(&ray.inv_dir);

	// use a div if inverted directions aren't available
	const __m256 l1 = mulps(subps(box_min, pos), inv_dir);
	const __m256 l2 = mulps(subps(box_max, pos), inv_dir);

	// the order we use for those min/max is vital to filter out
	// NaNs that happens when an inv_dir is +/- inf and
	// (box_min - pos) is 0. inf * 0 = NaN
	const __m256 filtered_l1a = minps(l1, plus_inf);
	const __m256 filtered_l2a = minps(l2, plus_inf);

	const __m256 filtered_l1b = maxps(l1, minus_inf);
	const __m256 filtered_l2b = maxps(l2, minus_inf);

	// now that we're back on our feet, test those slabs.
	////////////////////////////////////////////////////
	//__m256 lmax = maxps(filtered_l1a, filtered_l2a);
	//__m256 lmin = minps(filtered_l1b, filtered_l2b);
	__m128 l2a_128	= _mm256_castps256_ps128		(filtered_l2a);
	__m128 l2b_128	= _mm256_castps256_ps128		(filtered_l2b);
	__m128 l1a_128	= _mm256_castps256_ps128		(filtered_l1a);
	__m128 l1b_128	= _mm256_castps256_ps128		(filtered_l1b);
	__m128 lmax		= _mm_max_ps					(l1a_128, l2a_128);
	__m128 lmin		= _mm_min_ps					(l1b_128, l2b_128);
	////////////////////////////////////////////////////

	// unfold back. try to hide the latency of the shufps & co.
	const __m128 lmax0 = rotatelpssse(lmax);
	const __m128 lmin0 = rotatelpssse(lmin);

	lmax = _mm_min_ps(lmax, lmax0);
	lmin = _mm_max_ps(lmin, lmin0);

	// _mm_movehl_ps
	const __m128 lmax1 = muxhps(lmax, lmax);	
	const __m128 lmin1 = muxhps(lmin, lmin);


	lmax = _mm_min_ps(lmax, lmax1);
	lmin = _mm_max_ps(lmin, lmin1);



	storess(lmin, &dist);
	return !!(_mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax, lmin));

}

#else
ICF bool isect_sse(const aabb_t &box, const ray_t &ray, float &dist)
{
	// you may already have those values hanging around somewhere
	const __m128
		plus_inf = loadps(ps_cst_plus_inf),
		minus_inf = loadps(ps_cst_minus_inf);

	// use whatever's apropriate to load.
	const __m128
		box_min = loadps(&box.min),
		box_max = loadps(&box.max),
		pos = loadps(&ray.pos),
		inv_dir = loadps(&ray.inv_dir);

	// use a div if inverted directions aren't available
	const __m128 l1 = mulps(subps(box_min, pos), inv_dir);
	const __m128 l2 = mulps(subps(box_max, pos), inv_dir);

	// the order we use for those min/max is vital to filter out
	// NaNs that happens when an inv_dir is +/- inf and
	// (box_min - pos) is 0. inf * 0 = NaN
	const __m128 filtered_l1a = minps(l1, plus_inf);
	const __m128 filtered_l2a = minps(l2, plus_inf);

	const __m128 filtered_l1b = maxps(l1, minus_inf);
	const __m128 filtered_l2b = maxps(l2, minus_inf);

	// now that we're back on our feet, test those slabs.
	__m128 lmax = maxps(filtered_l1a, filtered_l2a);
	__m128 lmin = minps(filtered_l1b, filtered_l2b);

	// unfold back. try to hide the latency of the shufps & co.
	const __m128 lmax0 = rotatelps(lmax);
	const __m128 lmin0 = rotatelps(lmin);
	lmax = minss(lmax, lmax0);
	lmin = maxss(lmin, lmin0);

	const __m128 lmax1 = muxhps(lmax, lmax);
	const __m128 lmin1 = muxhps(lmin, lmin);
	lmax = minss(lmax, lmax1);
	lmin = maxss(lmin, lmin1);

	const bool ret = !!(_mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax, lmin));

	storess(lmin, &dist);
	//storess	(lmax, &rs.t_far);

	return  ret;
}
#endif

template <bool bCull, bool bFirst, bool bNearest>
class _MM_ALIGN16	ray_collider
{
public:
	COLLIDER*		dest;
	TRI*			tris;
	Fvector*		verts;
	
	ray_t			ray;
	float			rRange;
	float			rRange2;

	IC void _init(COLLIDER* CL, Fvector* V, TRI* T, const Fvector& C, const Fvector& D, float R)
	{
		dest			= CL;
		tris			= T;
		verts			= V;
		ray.pos.set		(C);
		ray.inv_dir.set	(1.f,1.f,1.f).div(D);
		ray.fwd_dir.set	(D);
		rRange			= R;
		rRange2			= R*R;
	}

	// sse


#ifdef __AVX_USE__

	ICF bool _box_sse(const Fvector& bCenter, const Fvector& bExtents, float&  dist)
	{
		aabb_t		box;
		__m128 CN = _mm_unpacklo_ps(_mm_load_ss((float*)&bCenter.x), _mm_load_ss((float*)&bCenter.y));
		CN = _mm_movelh_ps(CN, _mm_load_ss((float*)&bCenter.z));
		__m128 EX = _mm_unpacklo_ps(_mm_load_ss((float*)&bExtents.x), _mm_load_ss((float*)&bExtents.y));
		EX = _mm_movelh_ps(EX, _mm_load_ss((float*)&bExtents.z));

		_mm_store_ps((float*)&box.min, _mm_sub_ps(CN, EX));
		_mm_store_ps((float*)&box.max, _mm_add_ps(CN, EX));

		return isect_sse(box, ray, dist);
	}
	
#else
	ICF bool _box_sse(const Fvector& bCenter, const Fvector& bExtents, float&  dist)
	{
		aabb_t		box;
		__m128 CN = _mm_unpacklo_ps(_mm_load_ss((float*)&bCenter.x), _mm_load_ss((float*)&bCenter.y));
		CN = _mm_movelh_ps(CN, _mm_load_ss((float*)&bCenter.z));
		__m128 EX = _mm_unpacklo_ps(_mm_load_ss((float*)&bExtents.x), _mm_load_ss((float*)&bExtents.y));
		EX = _mm_movelh_ps(EX, _mm_load_ss((float*)&bExtents.z));

		_mm_store_ps((float*)&box.min, _mm_sub_ps(CN, EX));
		_mm_store_ps((float*)&box.max, _mm_add_ps(CN, EX));

		return isect_sse(box, ray, dist);
	}
#endif


	IC bool _tri(u32* p, float& u, float& v, float& range)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float	det,inv_det;
		
		// find vectors for two edges sharing vert0
		Fvector&			p0	= verts[ p[0] ];
		Fvector&			p1	= verts[ p[1] ];
		Fvector&			p2	= verts[ p[2] ];
		edge1.sub			(p1, p0);
		edge2.sub			(p2, p0);
		// begin calculating determinant - also used to calculate U parameter
		// if determinant is near zero, ray lies in plane of triangle
		pvec.crossproduct	(ray.fwd_dir, edge2);
		det = edge1.dotproduct(pvec);
		if (bCull)
		{						
			if (det < EPS)  return false;
			tvec.sub(ray.pos, p0);						// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec);					// calculate U parameter and test bounds
			if (u < 0.f || u > det) return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = ray.fwd_dir.dotproduct(qvec);			// calculate V parameter and test bounds
			if (v < 0.f || u + v > det) return false;
			range = edge2.dotproduct(qvec);				// calculate t, scale parameters, ray intersects triangle
			inv_det = 1.0f / det;
			range	*= inv_det;
			u		*= inv_det;
			v		*= inv_det;
		}
		else
		{			
			if (det > -EPS && det < EPS) return false;
			inv_det = 1.0f / det;
			tvec.sub(ray.pos, p0);						// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec)*inv_det;			// calculate U parameter and test bounds
			if (u < 0.0f || u > 1.0f)    return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = ray.fwd_dir.dotproduct(qvec)*inv_det;	// calculate V parameter and test bounds
			if (v < 0.0f || u + v > 1.0f) return false;
			range = edge2.dotproduct(qvec)*inv_det;		// calculate t, ray intersects triangle
		}
		return true;
	}
	
	void _prim(DWORD prim)
	{
		float	u,v,r;
		if (!_tri(tris[prim].verts, u, v, r))	return;
		if (r<=0 || r>rRange)					return;
		
		if (bNearest)	
		{
			if (dest->r_count())	
			{
				auto R_iter = dest->r_realBegin();
				if (r < R_iter->range)	{
					R_iter->id		= prim;
					R_iter->range		= r;
					R_iter->u			= u;
					R_iter->v			= v;
					R_iter->verts	[0]	= verts[tris[prim].verts[0]];
					R_iter->verts	[1]	= verts[tris[prim].verts[1]];
					R_iter->verts	[2]	= verts[tris[prim].verts[2]];
					R_iter->dummy		= tris[prim].dummy;
					rRange		= r;
					rRange2		= r*r;
				}
			} else {
				RESULT& R	= dest->r_add();
				R.id		= prim;
				R.range		= r;
				R.u			= u;
				R.v			= v;
				R.verts	[0]	= verts[tris[prim].verts[0]];
				R.verts	[1]	= verts[tris[prim].verts[1]];
				R.verts	[2]	= verts[tris[prim].verts[2]];
				R.dummy		= tris[prim].dummy;
				rRange		= r;
				rRange2		= r*r;
			}
		} else {
			RESULT& R	= dest->r_add();
			R.id		= prim;
			R.range		= r;
			R.u			= u;
			R.v			= v;
			R.verts	[0]	= verts[tris[prim].verts[0]];
			R.verts	[1]	= verts[tris[prim].verts[1]];
			R.verts	[2]	= verts[tris[prim].verts[2]];
			R.dummy		= tris[prim].dummy;
		}
	}
	void _stab(const AABBNoLeafNode* node)
	{

		// Here's the prefetchwt1, but can compile with SSE prefetcht1 or prefetcht2
		_mm_prefetch((char *)node->GetNeg(), _MM_HINT_NTA);

		// Actual ray/aabb test
		float d;
		if (!_box_sse((Fvector&)node->mAABB.mCenter, (Fvector&)node->mAABB.mExtents, d))
		{
			return;
		}
		if			(d  > rRange)		return;

		// 1st chield
		if (node->HasPosLeaf())	
			_prim((DWORD)node->GetPosPrimitive());
		else					
			_stab(node->GetPos());

		// Early exit for "only first"
		if (bFirst && dest->r_count())
			return;

		// 2nd chield
		if (node->HasNegLeaf())	
			_prim((DWORD)node->GetNegPrimitive());
		else					
			_stab(node->GetNeg());
	}
};

void COLLIDER::ray_query(const MODEL *m_def, const Fvector& r_start, const Fvector& r_dir, float r_range)
{
	m_def->syncronize();

	// Get nodes
	const AABBNoLeafNode* N = ((const CDB_OptimizeTree*)m_def->tree->GetTree())->GetNodes();

	r_clear();

	// Binary dispatcher
	if (ray_mode&OPT_CULL)
	{
		if (ray_mode&OPT_ONLYFIRST)
		{
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<true, true, true> RC;
				RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
				RC._stab(N);
			}
			else 
			{
				ray_collider<true, true, false> RC;
				RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
				RC._stab(N);
			}
		}
		else 
		{
			if (ray_mode&OPT_ONLYNEAREST) 
			{
				ray_collider<true, false, true>	RC;
				RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
				RC._stab(N);
			}
			else 
			{
				ray_collider<true, false, false> RC;
				RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
				RC._stab(N);
			}
		}
	}
	else
	{
		if (ray_mode&OPT_ONLYFIRST)
		{
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<false, true, true> RC;
				RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
				RC._stab(N);
			}
			else
			{
				ray_collider<false, true, false> RC;
				RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
				RC._stab(N);
			}
		}
		else 
		{
			if (ray_mode&OPT_ONLYNEAREST) 
			{
				ray_collider<false, false, true> RC;
				RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
				RC._stab(N);
			}
			else 
			{
				ray_collider<false, false, false> RC;
				RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
				RC._stab(N);
			}
		}
	}
}

