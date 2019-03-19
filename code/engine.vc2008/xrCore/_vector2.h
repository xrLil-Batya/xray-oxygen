#pragma once

template <class T>
struct _vector2 
{
public:
	typedef T			TYPE;
	typedef _vector2<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T x,y;

	// access operators
	IC T&		operator[]	(int i)						{ return (T&)*(&x + i); } // assert:  0 <= i < 2; x and y are packed into 2*sizeof(float) bytes
	IC T&		operator[]	(int i) const				{ return (T&)*(&x + i); } // assert:  0 <= i < 2; x and y are packed into 2*sizeof(float) bytes

	IC SelfRef	operator += (SelfCRef v)				{ return add(v); }
	IC SelfRef	operator += (const T p)					{ return add(p); }
	IC SelfRef	operator -= (SelfCRef v)				{ return sub(v); }
	IC SelfRef	operator -= (const T p)					{ return sub(p); }
	IC SelfRef	operator *= (SelfCRef v)				{ return mul(v); }
	IC SelfRef	operator *= (const T p)					{ return mul(p); }
	IC SelfRef	operator /= (SelfCRef v)				{ return div(v); }
	IC SelfRef	operator /= (const T p)					{ return div(p); }

	IC SelfRef	set		(const T _u, const T _v)		{ x = _u;			y = _v;					return *this; }
	IC SelfRef	set		(SelfCRef p)					{ x = p.x;			y = p.y;				return *this; }
	IC SelfRef	set		(const T* p)					{ x = p[0];			y = p[1];				return *this; }
	IC SelfRef	setZero	()								{ x = 0.0f;			y = 0.0f;				return *this; }

	IC SelfRef	add		(const T p)						{ x += p;			y += p;					return *this; }
	IC SelfRef	add		(SelfCRef p)					{ x += p.x;			y += p.y;				return *this; }
	IC SelfRef	add		(SelfCRef p1, SelfCRef p2)		{ x  = p1.x + p2.x;	y  = p1.y + p2.y;		return *this; }
	IC SelfRef	add		(SelfCRef p, float d)			{ x  = p.x + d;		y  = p.y + d;			return *this; }

	IC SelfRef	sub		(const T p)						{ x -= p;			y -= p;					return *this; }
	IC SelfRef	sub		(SelfCRef p)					{ x -= p.x;			y -= p.y;				return *this; }
	IC SelfRef	sub		(SelfCRef p1, SelfCRef p2)		{ x  = p1.x - p2.x; y  = p1.y - p2.y;		return *this; }
	IC SelfRef	sub		(SelfCRef p, float d)			{ x  = p.x - d;		y  = p.y - d;			return *this; }

	IC SelfRef	mul		(const T s)						{ x *= s;			y *= s;					return *this; }
	IC SelfRef	mul		(SelfCRef p)					{ x *= p.x;			y *= p.y;				return *this; }
	IC SelfRef	mul		(SelfCRef p1, SelfCRef p2)		{ x  = p1.x * p2.x; y  = p1.y * p2.y;		return *this; }
	IC SelfRef	mul		(SelfCRef p, float d)			{ x  = p.x * d;		y  = p.y * d;			return *this; }

	IC SelfRef	div		(const T s)						{ x /= s;			y /= s;					return *this; }
	IC SelfRef	div		(SelfCRef p)					{ x /= p.x;			y /= p.y;				return *this; }
	IC SelfRef	div		(SelfCRef p1, SelfCRef p2)		{ x  = p1.x / p2.x; y  = p1.y / p2.y;		return *this; }
	IC SelfRef	div		(SelfCRef p, float d)			{ x  = p.x / d;		y  = p.y / d;			return *this; }

	IC SelfRef	min		(SelfCRef p)					{ x = std::min(x,p.x);	y = std::min(y,p.y);	return *this; }
	IC SelfRef	min		(const T _x, const T _y)		{ x = std::min(x,_x);	y = std::min(y,_y);		return *this; }
	IC SelfRef	max		(SelfCRef p)					{ x = std::max(x,p.x);	y = std::max(y,p.y);	return *this; }
	IC SelfRef	max		(const T _x, const T _y)		{ x = std::max(x,_x);	y = std::max(y,_y);		return *this; }

	IC SelfRef	abs		()								{ x = _abs(x);			y = _abs(y);			return *this; }
	IC SelfRef	abs		(SelfCRef p)					{ x = _abs(p.x);		y = _abs(p.y);			return *this; }

	IC SelfRef	ceil	()								{ x = std::ceil(x);		y = std::ceil(y);		return *this; }
	IC SelfRef	ceil	(SelfCRef v)					{ x = std::ceil(v.x);	y = std::ceil(v.y);		return *this; }

	IC SelfRef	floor	()								{ x = std::floor(x);	y = std::floor(y);		return *this; }
	IC SelfRef	floor	(SelfCRef v)					{ x = std::floor(v.x);	y = std::floor(v.y);	return *this; }

	IC SelfRef	mad		(SelfCRef d, const T r)				{ x += d.x*r;			y += d.y*r;				return *this; }
    IC SelfRef	mad		(SelfCRef p, SelfCRef d, const T r)	{ x = p.x + d.x*r;		y = p.y + d.y*r;		return *this; }

	IC SelfRef	rot90	()								{ float t = -x; x = y; y = t;		return *this; }
	IC SelfRef	cross	(SelfCRef D)					{ x = D.y; y = -D.x;				return *this; }
	IC T		dot		(Self &p)						{ return x*p.x + y*p.y; }
	IC T		dot		(SelfCRef p) const				{ return x*p.x + y*p.y; }
	IC SelfRef	norm	()								{ float m = _sqrt(x*x + y*y); x /= m; y /= m;				return *this; }
	IC SelfRef	norm_safe()								{ float m = _sqrt(x*x + y*y); if (m) { x /= m; y /= m; }	return *this; }

	IC T		distance_to		(SelfCRef p) const		{ return _sqrt((x - p.x)*(x - p.x) + (y - p.y)*(y - p.y)); }
	IC T		square_magnitude() const				{ return x*x + y*y; }
	IC T		magnitude		() const				{ return _sqrt(square_magnitude()); }

    IC Self Cross()
	{
        // vector3 orthogonal to (x,y) is (y,-x)
        Self kCross;
        kCross.x = y;
        kCross.y = -x;
        return kCross;
    }

	IC bool similar(Self &p, const T eu, const T ev) const
	{ 
		return _abs(x - p.x) < eu && _abs(y - p.y) < ev;
	}
	
	IC bool similar(const Self &p, float E = EPS_L) const
	{ 
		return _abs(x - p.x) < E && _abs(y - p.y) < E;
	};

	// average arithmetic
	IC SelfRef averageA(Self &p1, Self &p2) 
	{
		x = (p1.x + p2.x)*T(0.5);
		y = (p1.y + p2.y)*T(0.5);
		return *this;	
	}
	// average geometric
	IC SelfRef averageG(Self &p1, Self &p2) 
	{
		x = _sqrt(p1.x*p2.x);
		y = _sqrt(p1.y*p2.y);
		return *this;	
	}

	IC SelfRef 	normalize		()						{ return norm();	 	}
	IC SelfRef 	normalize_safe	()						{ return norm_safe();	}
	IC SelfRef 	normalize		(const Self &v)			{ float m = _sqrt(v.x*v.x + v.y*v.y); x = v.x / m; y = v.y / m;				return *this; }
	IC SelfRef 	normalize_safe	(const Self &v)			{ float m = _sqrt(v.x*v.x + v.y*v.y); if (m) { x = v.x / m; y = v.y / m; }	return *this; }
	IC T 		dotproduct		(const Self &p) const	{ return dot(p);		}
	IC T 		crossproduct	(const Self &p) const	{ return y*p.x - x*p.y; }
	IC float 	getH			() const
	{
        if (fis_zero(y))
			if (fis_zero(x))
				return 0.0f;
			else
				return ((x > 0.0f) ? -PI_DIV_2 : PI_DIV_2);
		else
			if (y < 0.0f)
				return (-(atanf(x/y) - PI));
			else
				return (-atanf(x/y));
	}
};

using Fvector2 = _vector2<float>;
using Dvector2 = _vector2<double>;
using Ivector2 = _vector2<int>;

using Fvector2Vec = xr_vector<Fvector2>;

template <class T>
BOOL	_valid			(const _vector2<T>& v)	{ return _valid((T)v.x) && _valid((T)v.y);	}
