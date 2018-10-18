#include "stdafx.h"
#include "ISpatial.h"
#pragma warning(push)
#pragma warning(disable:4995)
#include <xmmintrin.h>
#pragma warning(pop)

// can you say "barebone"?
#ifndef _MM_ALIGN16
#	define _MM_ALIGN16		__declspec(align(16))
#endif // _MM_ALIGN16

struct	_MM_ALIGN16		vec_t : public Fvector3 {
	float		pad;
};

struct _MM_ALIGN16 aabb_t 
{
	vec_t		min;
	vec_t		max;
};
struct _MM_ALIGN16 ray_t 
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
#define minps				_mm256_min_ps
#define minpssse			_mm_min_ps
#define maxps				_mm256_max_ps
#define maxpssse			_mm_max_ps
#define mulps				_mm256_mul_ps
#define subps				_mm256_sub_ps
#define rotatelps(ps)		_mm256_shuffle_ps((ps),(ps), 0x39)		// a,b,c,d -> b,c,d,a
#define rotatelpssse(ps)	_mm_shuffle_ps((ps),(ps), 0x39)	
// NO ANALOG IN AVX
#define muxhps(low,high)	_mm_movehl_ps((low),(high))		// low{a,b,c,d}|high{e,f,g,h} = {c,d,g,h}


// SSE types
#define storess(ss,mem)		_mm_store_ss((float * const)(mem),(ss))
#define minss				_mm_min_ss
#define maxss				_mm_max_ss
#define m128_cast			_mm256_castps256_ps128
#else 

// turn those verbose intrinsics into something readable.
#define loadps(mem)			_mm_load_ps((const float * const)(mem))
#define storess(ss,mem)		_mm_store_ss((float * const)(mem),(ss))
#define minss				_mm_min_ss
#define maxss				_mm_max_ss
#define minps				_mm_min_ps
#define maxps				_mm_max_ps
#define mulps				_mm_mul_ps
#define subps				_mm_sub_ps
#define rotatelps(ps)		_mm_shuffle_ps((ps),(ps), 0x39)	// a,b,c,d -> b,c,d,a
#define muxhps(low,high)	_mm_movehl_ps((low),(high))		// low{a,b,c,d}|high{e,f,g,h} = {c,d,g,h}

#endif

static const float flt_plus_inf = -logf(0);	// let's keep C and C++ compilers happy.
static const float _MM_ALIGN16
ps_cst_plus_inf[4] = { flt_plus_inf,  flt_plus_inf,  flt_plus_inf,  flt_plus_inf },
ps_cst_minus_inf[4] = { -flt_plus_inf, -flt_plus_inf, -flt_plus_inf, -flt_plus_inf };

#ifdef __AVX_USE__
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
	//#VERTVER: return value must be __m128, not __m256
	return !!(_mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax, lmin));

}
#else
ICF bool isect_sse(const aabb_t &box, const ray_t &ray, float &dist) {
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
extern Fvector	c_spatial_offset[8];

template <bool b_first, bool b_nearest>
class	_MM_ALIGN16			walker
{
public:
	ray_t			ray;
	u32				mask;
	float			range;
	float			range2;
	ISpatial_DB*	space;
public:
	walker(ISpatial_DB*	_space, u32 _mask, const Fvector& _start, const Fvector&	_dir, float _range)
	{
		mask = _mask;
		ray.pos.set(_start);
		ray.inv_dir.set(1.f, 1.f, 1.f).div(_dir);
		ray.fwd_dir.set(_dir);

		range = _range;
		range2 = _range*_range;
		space = _space;
	}

	// sse
	ICF bool _box_sse(const Fvector& n_C, const float n_R, float&  dist)
	{
		aabb_t box;
		__m128 NR = _mm_load_ss((float*)&n_R);
		__m128 NC = _mm_unpacklo_ps(_mm_load_ss((float*)&n_C.x), _mm_load_ss((float*)&n_C.y));
		NR = _mm_add_ss(NR, NR);
		NC = _mm_movelh_ps(NC, _mm_load_ss((float*)&n_C.z));
		NR = _mm_shuffle_ps(NR, NR, _MM_SHUFFLE(1, 0, 0, 0));

		_mm_store_ps((float*)&box.min, _mm_sub_ps(NC, NR));
		_mm_store_ps((float*)&box.max, _mm_add_ps(NC, NR));

		return isect_sse(box, ray, dist);
	}
	void			walk(ISpatial_NODE* N, Fvector& n_C, float n_R)
	{
		// Actual ray/aabb test
		float d;
		if (!_box_sse(n_C, n_R, d))				return;
		if (d>range)							return;
		
		// test items
		for (auto _it = N->items.begin(); _it != N->items.end(); ++_it)
		{
			ISpatial*		S = *_it;
			if (mask != (S->spatial.type&mask))	continue;
			Fsphere&		sS = S->spatial.sphere;
			int				quantity;
			float			afT[2];
			Fsphere::ERP_Result	result = sS.intersect(ray.pos, ray.fwd_dir, range, quantity, afT);

			if (result == Fsphere::rpOriginInside || ((result == Fsphere::rpOriginOutside) && (afT[0]<range))) {
				if (b_nearest) {
					switch (result) {
					case Fsphere::rpOriginInside:	range = afT[0]<range ? afT[0] : range;	break;
					case Fsphere::rpOriginOutside:	range = afT[0];						break;
					}
					range2 = range*range;
				}
				space->q_result->push_back(S);
				if (b_first)				return;
			}
		}

		// recurse
		float	c_R = n_R / 2;
		for (u32 octant = 0; octant<8; octant++)
		{
			if (0 == N->children[octant])	continue;
			Fvector		c_C;			c_C.mad(n_C, c_spatial_offset[octant], c_R);
			walk(N->children[octant], c_C, c_R);
			if (b_first && !space->q_result->empty())	return;
		}
	}
};

void	ISpatial_DB::q_ray(xr_vector<ISpatial*>& R, u32 _o, u32 _mask_and, const Fvector&	_start, const Fvector&	_dir, float _range)
{
	xrCriticalSectionGuard guard(cs);
	q_result = &R;
	q_result->clear();

	if (_o & O_ONLYFIRST)
	{
		if (_o & O_ONLYNEAREST) { walker<true, true> W(this, _mask_and, _start, _dir, _range);	W.walk(m_root, m_center, m_bounds); }
		else { walker< true, false>	W(this, _mask_and, _start, _dir, _range);	W.walk(m_root, m_center, m_bounds); }
	}
	else 
	{
		if (_o & O_ONLYNEAREST) { walker<false, true> W(this, _mask_and, _start, _dir, _range);	W.walk(m_root, m_center, m_bounds); }
		else { walker<false, false>	W(this, _mask_and, _start, _dir, _range);	W.walk(m_root, m_center, m_bounds); }
	}
}
