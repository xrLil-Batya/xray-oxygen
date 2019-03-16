//////////////////////////////////////////////////////////////////////////
#pragma once
#define FLT_AS_DW(F) (*(DWORD*)&(F))
#define ALMOST_ZERO(F) ((FLT_AS_DW(F) & 0x7f800000L)==0)
#define IS_SPECIAL(F) ((FLT_AS_DW(F) & 0x7f800000L)==0x7f800000L)

struct Frustum
{
	inline Frustum();
	inline Frustum(const D3DXMATRIX* matrix);

	D3DXPLANE	camPlanes[6];
	int			nVertexLUT[6];
	D3DXVECTOR3 pntList[8];
};

struct BoundingBox
{
	D3DXVECTOR3 minPt;
	D3DXVECTOR3 maxPt;

	BoundingBox() : minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f) { }
	BoundingBox(const BoundingBox& other) : minPt(other.minPt), maxPt(other.maxPt) { }

	explicit BoundingBox(const D3DXVECTOR3* points, UINT n) : minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f)
	{
		for (unsigned int i = 0; i < n; i++)
			Merge(&points[i]);
	}

	explicit BoundingBox(const std::vector<D3DXVECTOR3>* points) : minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f)
	{
		for (unsigned int i = 0; i < points->size(); i++)
			Merge(&(*points)[i]);
	}
	explicit BoundingBox(const std::vector<BoundingBox>* boxes) : minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f)
	{
		for (unsigned int i = 0; i < boxes->size(); i++)
		{
			Merge(&(*boxes)[i].maxPt);
			Merge(&(*boxes)[i].minPt);
		}
	}
	void Centroid(D3DXVECTOR3* vec) const { *vec = 0.5f*(minPt + maxPt); }
	void Merge(const D3DXVECTOR3* vec)
	{
		minPt.x = std::min(minPt.x, vec->x);
		minPt.y = std::min(minPt.y, vec->y);
		minPt.z = std::min(minPt.z, vec->z);
		maxPt.x = std::max(maxPt.x, vec->x);
		maxPt.y = std::max(maxPt.y, vec->y);
		maxPt.z = std::max(maxPt.z, vec->z);
	}
	D3DXVECTOR3 Point(int i) const { return D3DXVECTOR3((i & 1) ? minPt.x : maxPt.x, (i & 2) ? minPt.y : maxPt.y, (i & 4) ? minPt.z : maxPt.z); }
};

///////////////////////////////////////////////////////////////////////////
inline BOOL LineIntersection2D(D3DXVECTOR2* result, const D3DXVECTOR2* lineA, const D3DXVECTOR2* lineB)
{
	//  if the lines are parallel, the lines will not intersect in a point
	//  NOTE: assumes the rays are already normalized!!!!
	VERIFY(_abs(D3DXVec2Dot(&lineA[1], &lineB[1])) < 1.f);

	float x[2] = { lineA[0].x, lineB[0].x };
	float y[2] = { lineA[0].y, lineB[0].y };
	float dx[2] = { lineA[1].x, lineB[1].x };
	float dy[2] = { lineA[1].y, lineB[1].y };

	float x_diff = x[0] - x[1];
	float y_diff = y[0] - y[1];

	float s = (x_diff - (dx[1] / dy[1])*y_diff) / ((dx[1] * dy[0] / dy[1]) - dx[0]);
	// float t	= (x_diff + s*dx[0]) / dx[1];

	*result = lineA[0] + s * lineA[1];
	return		TRUE;
}
///////////////////////////////////////////////////////////////////////////
//  PlaneIntersection
//    computes the point where three planes intersect
//    returns whether or not the point exists.
static inline BOOL PlaneIntersection(D3DXVECTOR3* intersectPt, const D3DXPLANE* p0, const D3DXPLANE* p1, const D3DXPLANE* p2)
{
	D3DXVECTOR3 n0(p0->a, p0->b, p0->c);
	D3DXVECTOR3 n1(p1->a, p1->b, p1->c);
	D3DXVECTOR3 n2(p2->a, p2->b, p2->c);

	D3DXVECTOR3 n1_n2, n2_n0, n0_n1;

	D3DXVec3Cross(&n1_n2, &n1, &n2);
	D3DXVec3Cross(&n2_n0, &n2, &n0);
	D3DXVec3Cross(&n0_n1, &n0, &n1);

	float cosTheta = D3DXVec3Dot(&n0, &n1_n2);

	if (ALMOST_ZERO(cosTheta) || IS_SPECIAL(cosTheta))
		return FALSE;

	float secTheta = 1.f / cosTheta;

	n1_n2 = n1_n2 * p0->d;
	n2_n0 = n2_n0 * p1->d;
	n0_n1 = n0_n1 * p2->d;

	*intersectPt = -(n1_n2 + n2_n0 + n0_n1) * secTheta;
	return TRUE;
}

inline Frustum::Frustum()
{
	for (int i = 0; i < 6; i++)
		camPlanes[i] = D3DXPLANE(0.f, 0.f, 0.f, 0.f);
}

//  build a frustum from a camera (projection, or viewProjection) matrix
inline Frustum::Frustum(const D3DXMATRIX* matrix)
{
	//  build a view frustum based on the current view & projection matrices...
	D3DXVECTOR4 column4(matrix->_14, matrix->_24, matrix->_34, matrix->_44);
	D3DXVECTOR4 column1(matrix->_11, matrix->_21, matrix->_31, matrix->_41);
	D3DXVECTOR4 column2(matrix->_12, matrix->_22, matrix->_32, matrix->_42);
	D3DXVECTOR4 column3(matrix->_13, matrix->_23, matrix->_33, matrix->_43);

	D3DXVECTOR4 planes[6];
	planes[0] = column4 - column1;  // left
	planes[1] = column4 + column1;  // right
	planes[2] = column4 - column2;  // bottom
	planes[3] = column4 + column2;  // top
	planes[4] = column4 - column3;  // near
	planes[5] = column4 + column3;  // far
	// ignore near & far plane

	int p;

	for (p = 0; p < 6; p++)  // normalize the planes
	{
		float dot = planes[p].x*planes[p].x + planes[p].y*planes[p].y + planes[p].z*planes[p].z;
		dot = 1.f / _sqrt(dot);
		planes[p] = planes[p] * dot;
	}

	for (p = 0; p < 6; p++)
		camPlanes[p] = D3DXPLANE(planes[p].x, planes[p].y, planes[p].z, planes[p].w);

	//  build a bit-field that will tell us the indices for the nearest and farthest vertices from each plane...
	for (int i = 0; i < 6; i++)
		nVertexLUT[i] = ((planes[i].x < 0.f) ? 1 : 0) | ((planes[i].y < 0.f) ? 2 : 0) | ((planes[i].z < 0.f) ? 4 : 0);

	for (int i = 0; i < 8; i++)  // compute extrema
	{
		const D3DXPLANE& p0 = (i & 1) ? camPlanes[4] : camPlanes[5];
		const D3DXPLANE& p1 = (i & 2) ? camPlanes[3] : camPlanes[2];
		const D3DXPLANE& p2 = (i & 4) ? camPlanes[0] : camPlanes[1];
		PlaneIntersection(&pntList[i], &p0, &p1, &p2);
	}
}

//////////////////////////////////////////////////////////////////////////
inline Fvector3 wform(Fmatrix& m, Fvector3 const& v)
{
	Fvector4	r;
	r.x = v.x*m._11 + v.y*m._21 + v.z*m._31 + m._41;
	r.y = v.x*m._12 + v.y*m._22 + v.z*m._32 + m._42;
	r.z = v.x*m._13 + v.y*m._23 + v.z*m._33 + m._43;
	r.w = v.x*m._14 + v.y*m._24 + v.z*m._34 + m._44;
	// VERIFY		(r.w>0.f);
	float invW = 1.0f / r.w;
	Fvector3	r3 = { r.x*invW, r.y*invW, r.z*invW };
	return		r3;
}

//////////////////////////////////////////////////////////////////////////
// OLES: naive 3D clipper - roubustness around 0, but works for this sample
// note: normals points to 'outside'
//////////////////////////////////////////////////////////////////////////
const	float	_eps = 0.000001f;
struct	DumbClipper
{
	CFrustum				frustum;
	xr_vector<D3DXPLANE>	planes;
	BOOL					clip(D3DXVECTOR3& p0, D3DXVECTOR3& p1)		// returns TRUE if result meaningfull
	{
		float		denum;
		D3DXVECTOR3	D;
		for (int it = 0; it<int(planes.size()); it++)
		{
			D3DXPLANE&	P = planes[it];
			float		cls0 = D3DXPlaneDotCoord(&P, &p0);
			float		cls1 = D3DXPlaneDotCoord(&P, &p1);
			if (cls0 > 0 && cls1 > 0)	return			false;	// fully outside

			if (cls0 > 0) {
				// clip p0
				D = p1 - p0;
				denum = D3DXPlaneDotNormal(&P, &D);
				if (denum != 0) p0 += -D * cls0 / denum;
			}
			if (cls1 > 0) {
				// clip p1
				D = p0 - p1;
				denum = D3DXPlaneDotNormal(&P, &D);
				if (denum != 0) p1 += -D * cls1 / denum;
			}
		}
		return	true;
	}
	D3DXVECTOR3			point(Fbox& bb, int i) const { return D3DXVECTOR3((i & 1) ? bb.min.x : bb.max.x, (i & 2) ? bb.min.y : bb.max.y, (i & 4) ? bb.min.z : bb.max.z); }
	Fbox				clipped_AABB(xr_vector<Fbox, xalloc<Fbox3> >& src, Fmatrix& xf)
	{
		Fbox3		result;		result.invalidate();
		for (int it = 0; it<int(src.size()); it++) {
			Fbox&			bb = src[it];
			u32				mask = frustum.getMask();
			EFC_Visible		res = frustum.testAABB(&bb.min.x, mask);
			switch (res)
			{
			case fcvNone:	continue;
			case fcvFully:
				for (int c = 0; c < 8; c++)
				{
					D3DXVECTOR3		p0 = point(bb, c);
					Fvector			x0 = wform(xf, *((Fvector*)(&p0)));
					result.modify(x0);
				}
				break;
			case fcvPartial:
				for (int c0 = 0; c0 < 8; c0++)
				{
					for (int c1 = 0; c1 < 8; c1++)
					{
						if (c0 == c1)			continue;
						D3DXVECTOR3		p0 = point(bb, c0);
						D3DXVECTOR3		p1 = point(bb, c1);
						if (!clip(p0, p1))	continue;
						Fvector			x0 = wform(xf, *((Fvector*)(&p0)));
						Fvector			x1 = wform(xf, *((Fvector*)(&p1)));
						result.modify(x0);
						result.modify(x1);
					}
				}
				break;
			};
		}
		return			result;
	}
};

template <class _Tp>
inline const _Tp& min(const _Tp& __a, const _Tp& __b) {
	return __b < __a ? __b : __a;
}
template <class _Tp>
inline const _Tp& max(const _Tp& __a, const _Tp& __b) {
	return  __a < __b ? __b : __a;
}

extern xr_vector<Fbox, xalloc<Fbox>> s_casters;

inline D3DXVECTOR2 BuildTSMProjectionMatrix_caster_depth_bounds(D3DXMATRIX& lightSpaceBasis)
{
	float		min_z = 1e32f, max_z = -1e32f;
	D3DXMATRIX	minmax_xf;
	D3DXMatrixMultiply(&minmax_xf, (D3DXMATRIX*)&Device.mView, &lightSpaceBasis);
	Fmatrix&	minmax_xform = *((Fmatrix*)&minmax_xf);
	for (u32 c = 0; c < s_casters.size(); c++)
	{
		Fvector3	pt;
		for (int e = 0; e < 8; e++) {
			s_casters[c].getpoint(e, pt);
			pt = wform(minmax_xform, pt);
			min_z = std::min(min_z, pt.z);
			max_z = std::max(max_z, pt.z);
		}
	}
	return D3DXVECTOR2(min_z, max_z);
}

template <bool _debug>
class	DumbConvexVolume
{
public:
	struct	_poly
	{
		xr_vector<int>	points;
		Fvector3		planeN;
		float			planeD;
		float			classify(Fvector3& p) { return planeN.dotproduct(p) + planeD; }
	};
	struct	_edge
	{
		int				p0, p1;
		int				counter;
		_edge(int _p0, int _p1, int m) : p0(_p0), p1(_p1), counter(m) { if (p0 > p1)	swap(p0, p1); }
		bool			equal(_edge& E) { return p0 == E.p0 && p1 == E.p1; }
	};
public:
	xr_vector<Fvector3>		points;
	xr_vector<_poly>		polys;
	xr_vector<_edge>		edges;
public:
	void				compute_planes()
	{
		for (int it = 0; it<int(polys.size()); it++)
		{
			_poly&			P = polys[it];
			Fvector3		t1, t2;
			float			len;

			t1.sub(points[P.points[0]], points[P.points[1]]);
			t2.sub(points[P.points[0]], points[P.points[2]]);

			P.planeN.crossproduct(t1, t2);

			len = P.planeN.magnitude();

			if (len > std::numeric_limits<float>::min())
			{
				P.planeN.mul(1 / len);
			}
			else
			{
				t2.sub(points[P.points[0]], points[P.points[3]]);
				P.planeN.crossproduct(t1, t2);
				if (len > std::numeric_limits<float>::min())
				{
					P.planeN.mul(1 / len);
				}
				else
				{
					//	HACK:	Remove plane.
					//VERIFY(!"Can't build normal to plane!");
					polys.erase(polys.begin() + it);
					--it;
					continue;
				}
			}

			P.planeD = -P.planeN.dotproduct(points[P.points[0]]);

			// verify
			if (_debug)
			{
				Fvector&		p0 = points[P.points[0]];
				Fvector&		p1 = points[P.points[1]];
				Fvector&		p2 = points[P.points[2]];
				Fvector&		p3 = points[P.points[3]];
				Fplane	p012;	p012.build(p0, p1, p2);
				Fplane	p123;	p123.build(p1, p2, p3);
				Fplane	p230;	p230.build(p2, p3, p0);
				Fplane	p301;	p301.build(p3, p0, p1);
				VERIFY(p012.n.similar(p123.n) && p012.n.similar(p230.n) && p012.n.similar(p301.n));
			}
		}
	}
	void				compute_caster_model(xr_vector<Fplane>& dest, Fvector3 direction)
	{
		CRenderTarget&	T = *RImplementation.Target;

		// COG
		Fvector3	cog = { 0,0,0 };
		for (int it = 0; it<int(points.size()); it++)	cog.add(points[it]);
		cog.div(float(points.size()));

		// planes
		compute_planes();
		for (int it = 0; it<int(polys.size()); it++)
		{
			_poly&	base = polys[it];
			if (base.classify(cog) > 0)	std::reverse(base.points.begin(), base.points.end());
		}

		// remove faceforward polys, build list of edges -> find open ones
		compute_planes();
		for (int it = 0; it<int(polys.size()); it++)
		{
			_poly&	base = polys[it];
			VERIFY(base.classify(cog) < 0);								// debug

			int		marker = (base.planeN.dotproduct(direction) <= 0) ? -1 : 1;

			// register edges
			xr_vector<int>&	plist = polys[it].points;
			for (int p = 0; p<int(plist.size()); p++) {
				_edge	E(plist[p], plist[(p + 1) % plist.size()], marker);
				bool	found = false;
				for (int e = 0; e<int(edges.size()); e++)
					if (edges[e].equal(E)) { edges[e].counter += marker; found = true; break; }
				if (!found) {
					edges.push_back(E);
					if (_debug)	T.dbg_addline(points[E.p0], points[E.p1], color_rgba(255, 0, 0, 255));
				}
			}

			// remove if unused
			if (marker < 0) {
				polys.erase(polys.begin() + it);
				it--;
			}
		}

		// Extend model to infinity, the volume is not capped, so this is indeed up to infinity
		for (int e = 0; e<int(edges.size()); e++)
		{
			if (edges[e].counter != 0)	continue;
			_edge&		E = edges[e];
			if (_debug)	T.dbg_addline(points[E.p0], points[E.p1], color_rgba(255, 255, 255, 255));
			Fvector3	point;
			points.push_back(point.sub(points[E.p0], direction));
			points.push_back(point.sub(points[E.p1], direction));
			polys.push_back(_poly());
			_poly&		P = polys.back();
			int			pend = int(points.size());
			P.points.push_back(E.p0);
			P.points.push_back(E.p1);
			P.points.push_back(pend - 1);	//p1 mod
			P.points.push_back(pend - 2);	//p0 mod
			if (_debug)	T.dbg_addline(points[E.p0], point.mad(points[E.p0], direction, -1000), color_rgba(0, 255, 0, 255));
			if (_debug)	T.dbg_addline(points[E.p1], point.mad(points[E.p1], direction, -1000), color_rgba(0, 255, 0, 255));
		}

		// Reorient planes (try to write more inefficient code :)
		compute_planes();
		for (int it = 0; it<int(polys.size()); it++)
		{
			_poly&	base = polys[it];
			if (base.classify(cog) > 0)	std::reverse(base.points.begin(), base.points.end());
		}

		// Export
		compute_planes();
		for (int it = 0; it<int(polys.size()); it++)
		{
			_poly&			P = polys[it];
			Fplane			pp = { P.planeN,P.planeD };
			dest.push_back(pp);
		}
	}
};


const inline u32 LIGHT_CUBOIDSIDEPOLYS_COUNT = 4;
const inline u32 LIGHT_CUBOIDVERTICES_COUNT = 2 * LIGHT_CUBOIDSIDEPOLYS_COUNT;

template <bool _debug>
class	FixedConvexVolume
{
public:

	struct	_poly
	{
		int				points[4];
		Fplane			plane;
	};

	xr_vector<sun::ray>			view_frustum_rays;
	sun::ray					view_ray;
	sun::ray					light_ray;
	Fvector3					light_cuboid_points[LIGHT_CUBOIDVERTICES_COUNT];
	_poly						light_cuboid_polys[LIGHT_CUBOIDSIDEPOLYS_COUNT];

public:

	void				compute_planes()
	{
		for (u32 it = 0; it < LIGHT_CUBOIDSIDEPOLYS_COUNT; it++)
		{
			_poly&			P = light_cuboid_polys[it];

			P.plane.build(light_cuboid_points[P.points[0]], light_cuboid_points[P.points[2]], light_cuboid_points[P.points[1]]);

			// verify
			if (_debug)
			{
				Fvector&		p0 = light_cuboid_points[P.points[0]];
				Fvector&		p1 = light_cuboid_points[P.points[1]];
				Fvector&		p2 = light_cuboid_points[P.points[2]];
				Fvector&		p3 = light_cuboid_points[P.points[3]];
				Fplane	p012;	p012.build(p0, p1, p2);
				Fplane	p123;	p123.build(p1, p2, p3);
				Fplane	p230;	p230.build(p2, p3, p0);
				Fplane	p301;	p301.build(p3, p0, p1);
				VERIFY(p012.n.similar(p123.n) && p012.n.similar(p230.n) && p012.n.similar(p301.n));
			}
		}
	}

	void		compute_caster_model_fixed(xr_vector<Fplane>& dest, Fvector3& translation, float map_size, bool clip_by_view_near)
	{
		translation.set(0.f, 0.f, 0.f);

		if (fis_zero(1 - abs(view_ray.D.dotproduct(light_ray.D)), EPS_S))
			return;

		// compute planes for each polygon.
		compute_planes();

		for (u32 i = 0; i < LIGHT_CUBOIDSIDEPOLYS_COUNT; i++)
			VERIFY(light_cuboid_polys[i].plane.classify(light_ray.P) > 0);

		int	align_planes[2];
		int	align_planes_count = 0;

		// find one or two planes that align to view frustum from behind. 
		for (u32 i = 0; i < LIGHT_CUBOIDSIDEPOLYS_COUNT; i++)
		{
			float tmp_dot = view_ray.D.dotproduct(light_cuboid_polys[i].plane.n);
			if (tmp_dot <= EPS_L)
				continue;

			align_planes[align_planes_count] = i;
			++align_planes_count;

			if (align_planes_count == 2)
				break;
		}


		Fvector align_vector;
		align_vector.set(0.f, 0.f, 0.f);

		// Align ray points to the align planes.
		for (int p = 0; p < align_planes_count; ++p)
		{
			// Hack !
			float min_dist = 10000;
			for (u32 i = 0; i < view_frustum_rays.size(); ++i)
			{
				float tmp_dist = 0;
				Fvector tmp_point = view_frustum_rays[i].P;

				tmp_dist = light_cuboid_polys[align_planes[p]].plane.classify(tmp_point);
				min_dist = std::min(tmp_dist, min_dist);
			}

			Fvector shift = light_cuboid_polys[align_planes[p]].plane.n;
			shift.mul(min_dist);
			align_vector.add(shift);
		}

		translation.add(align_vector);

		// Move light ray by the alignment shift.
		light_ray.P.add(align_vector);

		// Here we can skip this stage us in the next pass we need only normals of planes.
		// in the next translate_light_model call will contain this shift as well.
		// translate_light_model	( align_vector );

		// Reset to reuse.
		align_vector.set(0.f, 0.f, 0.f);

		// Check if view edges intersect, and push planes................ 
		for (int p = 0; p < align_planes_count; ++p)
		{
			float max_mag = 0;
			for (u32 i = 0; i < view_frustum_rays.size(); ++i)
			{
				float plane_dot_ray = view_frustum_rays[i].D.dotproduct(light_cuboid_polys[align_planes[p]].plane.n);
				if (plane_dot_ray < 0)
				{
					Fvector per_plane_view;
					per_plane_view.crossproduct(light_cuboid_polys[align_planes[p]].plane.n, view_ray.D);
					Fvector per_view_to_plane;
					per_view_to_plane.crossproduct(per_plane_view, view_ray.D);

					float tmp_mag = -plane_dot_ray / view_frustum_rays[i].D.dotproduct(per_view_to_plane);

					max_mag = (max_mag < tmp_mag) ? tmp_mag : max_mag;
				}
			}

			if (fis_zero(max_mag))
				continue;

			VERIFY(max_mag <= 1.f);

			float dist = -light_cuboid_polys[align_planes[p]].plane.n.dotproduct(translation);
			align_vector.mad(light_cuboid_polys[align_planes[p]].plane.n, dist*max_mag);
		}

		translation.add(align_vector);
		light_ray.P.add(align_vector);
		translate_light_model(translation);


		// compute culling planes by rays as edges
		for (u32 i = 0; i < view_frustum_rays.size(); ++i)
		{
			Fvector tmp_vector;
			tmp_vector.crossproduct(view_frustum_rays[i].D, light_ray.D);

			// check if the vectors are parallel
			if (fis_zero(tmp_vector.square_magnitude(), EPS))
				continue;

			Fplane tmp_plane;
			tmp_plane.build(view_frustum_rays[i].P, tmp_vector);

			float sign = 0;
			if (check_cull_plane_valid(tmp_plane, sign, 5))
			{
				tmp_plane.n.mul(-sign);
				tmp_plane.d *= -sign;
				dest.push_back(tmp_plane);
			}
		}

		// compute culling planes by ray points pairs as edges
		if (clip_by_view_near && abs(view_ray.D.dotproduct(light_ray.D)) < 0.8)
		{
			Fvector perp_light_view, perp_light_to_view;
			perp_light_view.crossproduct(view_ray.D, light_ray.D);
			perp_light_to_view.crossproduct(perp_light_view, light_ray.D);

			Fplane plane;
			plane.build(view_ray.P, perp_light_to_view);

			float max_dist = -1000;
			for (u32 i = 0; i < view_frustum_rays.size(); ++i)
				max_dist = std::max(plane.classify(view_frustum_rays[i].P), max_dist);

			for (u32 i = 0; i < view_frustum_rays.size(); ++i)
			{
				Fvector P = view_frustum_rays[i].P;
				P.mad(view_frustum_rays[i].D, 5);

				if (plane.classify(P) > max_dist)
				{
					max_dist = 0.f;
					break;
				}
			}

			if (max_dist > -1000)
			{
				plane.d += max_dist;
				dest.push_back(plane);
			}
		}

		for (u32 i = 0; i < LIGHT_CUBOIDSIDEPOLYS_COUNT; i++)
		{
			dest.push_back(light_cuboid_polys[i].plane);
			dest.back().n.mul(-1);
			dest.back().d *= -1;
			VERIFY(light_cuboid_polys[i].plane.classify(light_ray.P) > 0);
		}

		// Compute ray intersection with light model, this is needed to next cascade to start it's placement.
		for (u32 i = 0; i < view_frustum_rays.size(); ++i)
		{
			float min_dist = 2 * map_size;
			for (int p = 0; p < 4; ++p)
			{
				float dist;
				if ((light_cuboid_polys[p].plane.n.dotproduct(view_frustum_rays[i].D)) > -0.1)
					dist = map_size;
				else
					light_cuboid_polys[p].plane.intersectRayDist(view_frustum_rays[i].P, view_frustum_rays[i].D, dist);

				if (dist > EPS_L &&  dist < min_dist)
					min_dist = dist;
			}

			view_frustum_rays[i].P.mad(view_frustum_rays[i].D, min_dist);
		}
	}

	bool check_cull_plane_valid(Fplane const &plane, float &sign, float mad_factor = 0.f)
	{
		bool valid = false;
		bool oriented = false;
		float orient = 0;
		for (u32 j = 0; j < view_frustum_rays.size(); ++j)
		{
			float tmp_dist = 0.f;
			Fvector tmp_pt = view_frustum_rays[j].P;
			tmp_pt.mad(view_frustum_rays[j].D, mad_factor);
			tmp_dist = plane.classify(tmp_pt);

			if (fis_zero(tmp_dist, EPS_L))
				continue;

			if (!oriented)
			{
				orient = tmp_dist > 0.f ? 1.f : -1.f;
				valid = true;
				oriented = true;
				continue;
			}

			if (tmp_dist < 0 && orient < 0 || tmp_dist > 0 && orient > 0)
				continue;

			valid = false;
			break;
		}
		sign = orient;
		return valid;
	}

	void translate_light_model(Fvector translate)
	{
		Fmatrix trans_mat; trans_mat.translate(translate);
		for (int i = 0; i < LIGHT_CUBOIDSIDEPOLYS_COUNT; ++i)
			light_cuboid_polys[i].plane.d -= translate.dotproduct(light_cuboid_polys[i].plane.n);
	}
};
