#ifndef __V2D__
#define __V2D__

template <class T>
struct _vector2
{
public:
	typedef T			TYPE;
	typedef _vector2<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T x, y;

	inline SelfRef set(float _u, float _v) { x = T(_u); y = T(_v); return *this; }
	inline SelfRef set(double _u, double _v) { x = T(_u); y = T(_v); return *this; }
	inline SelfRef set(int _u, int _v) { x = T(_u); y = T(_v); return *this; }
	inline SelfRef set(const Self &p) { x = p.x; y = p.y; return *this; }
	inline SelfRef abs(const Self &p) { x = _abs(p.x); y = _abs(p.y); return *this; }
	inline SelfRef min(const Self &p) { x = std::min(x, p.x); y = std::min(y, p.y);	return *this; }
	inline SelfRef min(T _x, T _y) { x = std::min(x, _x);  y = std::min(y, _y);	return *this; }
	inline SelfRef max(const Self &p) { x = std::max(x, p.x); y = std::max(y, p.y);	return *this; }
	inline SelfRef max(T _x, T _y) { x = std::max(x, _x);  y = std::max(y, _y);	return *this; }
	inline SelfRef sub(const T p) { x -= p; y -= p; return *this; }
	inline SelfRef sub(const Self &p) { x -= p.x; y -= p.y; return *this; }
	inline SelfRef sub(const Self &p1, const Self &p2) { x = p1.x - p2.x; y = p1.y - p2.y; return *this; }
	inline SelfRef sub(const Self &p, float d) { x = p.x - d; y = p.y - d; return *this; }
	inline SelfRef add(const T p) { x += p; y += p; return *this; }
	inline SelfRef add(const Self &p) { x += p.x; y += p.y; return *this; }
	inline SelfRef add(const Self &p1, const Self &p2) { x = p1.x + p2.x; y = p1.y + p2.y; return *this; }
	inline SelfRef add(const Self &p, float d) { x = p.x + d; y = p.y + d; return *this; }
	inline SelfRef mul(const T s) { x *= s; y *= s; return *this; }
	inline SelfRef mul(const Self &p) { x *= p.x; y *= p.y; return *this; }
	inline SelfRef div(const T s) { x /= s; y /= s; return *this; }
	inline SelfRef div(const Self &p) { x /= p.x; y /= p.y; return *this; }
	inline SelfRef rot90(void) { float t = -x; x = y; y = t; return *this; }
	inline SelfRef cross(const Self &D) { x = D.y; y = -D.x; return *this; }
	inline T dot(Self &p) { return x * p.x + y * p.y; }
	inline T dot(const Self &p) const { return x * p.x + y * p.y; }
	inline SelfRef norm(void) { float m = _sqrt(x*x + y * y); x /= m; y /= m; return *this; }
	inline SelfRef norm_safe(void) { float m = _sqrt(x*x + y * y); if (m) { x /= m; y /= m; }	return *this; }
	inline T distance_to(const Self &p) const { return _sqrt((x - p.x)*(x - p.x) + (y - p.y)*(y - p.y)); }
	inline T square_magnitude(void) const { return x * x + y * y; }
	inline T magnitude(void) const { return _sqrt(square_magnitude()); }

	inline SelfRef mad(const Self &p, const Self& d, T r)
	{
		x = p.x + d.x*r;
		y = p.y + d.y*r;
		return *this;
	}
	inline Self Cross()
	{
		// vector3 orthogonal to (x,y) is (y,-x)
		Self kCross;
		kCross.x = y;
		kCross.y = -x;
		return kCross;
	}

	inline bool similar(Self &p, T eu, T ev) const
	{
		return _abs(x - p.x) < eu && _abs(y - p.y) < ev;
	}

	inline bool similar(const Self &p, float E = EPS_L) const
	{
		return _abs(x - p.x) < E && _abs(y - p.y) < E;
	};

	// average arithmetic
	inline SelfRef averageA(Self &p1, Self &p2)
	{
		x = (p1.x + p2.x)*.5f;
		y = (p1.y + p2.y)*.5f;
		return *this;
	}
	// average geometric
	inline SelfRef averageG(Self &p1, Self &p2)
	{
		x = _sqrt(p1.x*p2.x);
		y = _sqrt(p1.y*p2.y);
		return *this;
	}

	T& operator[] (int i) const
	{
		// assert:  0 <= i < 2; x and y are packed into 2*sizeof(float) bytes
		return (T&) *(&x + i);
	}

	inline SelfRef normalize(void) { return norm(); }
	inline SelfRef normalize_safe(void) { return norm_safe(); }
	inline SelfRef normalize(const Self &v) { float m = _sqrt(v.x*v.x + v.y*v.y); x = v.x / m; y = v.y / m; return *this; }
	inline SelfRef normalize_safe(const Self &v) { float m = _sqrt(v.x*v.x + v.y*v.y); if (m) { x = v.x / m; y = v.y / m; }	return *this; }
	inline float dotproduct(const Self &p) const { return dot(p); }
	inline float crossproduct(const Self &p) const { return y * p.x - x * p.y; }
	inline float getH(void) const
	{
		if (fis_zero(y))
			if (fis_zero(x))
				return (0.f);
			else
				return ((x > 0.0f) ? -PI_DIV_2 : PI_DIV_2);
		else
			if (y < 0.f)
				return (-(atanf(x / y) - PI));
			else
				return (-atanf(x / y));
	}
};

typedef _vector2<float> Fvector2;
typedef _vector2<double> Dvector2;
typedef _vector2<int> Ivector2;

template <class T>
BOOL _valid(const _vector2<T>& v) { return _valid((T)v.x) && _valid((T)v.y); }

#endif