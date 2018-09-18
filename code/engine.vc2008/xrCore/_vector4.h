#pragma once

template <class T>
struct _vector4
{
	typedef T			TYPE;
	typedef _vector4<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T x,y,z,w;

	// access operators
	IC	T&		operator[]	(int i)						{ return *((T*)this + i); }
	IC	T&		operator[]	(int i)	const				{ return *((T*)this + i); }

	IC SelfRef	operator += (SelfCRef v)				{ return add(v); }
	IC SelfRef	operator += (const T p)					{ return add(p); }
	IC SelfRef	operator -= (SelfCRef v)				{ return sub(v); }
	IC SelfRef	operator -= (const T p)					{ return sub(p); }
	IC SelfRef	operator *= (SelfCRef v)				{ return mul(v); }
	IC SelfRef	operator *= (const T p)					{ return mul(p); }
	IC SelfRef	operator /= (SelfCRef v)				{ return div(v); }
	IC SelfRef	operator /= (const T p)					{ return div(p); }

	IC	SelfRef	set		(T _x, T _y, T _z, T _w = 1)	{ x = _x;			y = _y;			z = _z;			w = _w;			return *this; }
	IC	SelfRef	set		(SelfCRef v)					{ x = v.x;			y = v.y;		z = v.z;		w = v.w;		return *this; }
	ICF	SelfRef	set		(T* p)							{ x = p[0];			y = p[1];		z = p[2];		w = p[3];		return *this; }

	IC	SelfRef	add		(T _x, T _y, T _z, T _w = 1)	{ x += _x;			y += _y;		z += _z;		w += _w;		return *this; }
	IC	SelfRef	add		(SelfCRef v)					{ x += v.x;			y += v.y;		z += v.z;		w += v.w;		return *this; }
	IC  SelfRef	add		(T s)							{ x += s;			y += s;			z += s;			w += s;			return *this; }
	IC	SelfRef	add		(SelfCRef a, SelfCRef v)		{ x  = a.x + v.x;	y  = a.y + v.y;	z  = a.z + v.z;	w  = a.w + v.w;	return *this; }
	IC  SelfRef	add		(SelfCRef a, T s)				{ x  = a.x + s;		y  = a.y + s;	z  = a.z + s;	w  = a.w + s;	return *this; }

	IC	SelfRef	sub		(T _x, T _y, T _z, T _w = 1)	{ x -= _x;			y -= _y;		z -= _z;		w -= _w;		return *this; }
	IC	SelfRef	sub		(SelfCRef v)					{ x -= v.x;			y -= v.y;		z -= v.z;		w -= v.w;		return *this; }
	IC  SelfRef	sub		(T s)							{ x -= s;			y -= s;			z -= s;			w -= s;			return *this; }
	IC	SelfRef	sub		(SelfCRef a, SelfCRef v)		{ x  = a.x - v.x;	y  = a.y - v.y;	z  = a.z - v.z;	w  = a.w - v.w;	return *this; }
	IC  SelfRef	sub		(SelfCRef a, T s)				{ x  = a.x - s;		y  = a.y - s;	z  = a.z - s;	w  = a.w - s;	return *this; }

	IC	SelfRef	mul		(T _x, T _y, T _z, T _w = 1)	{ x *= _x;			y *= _y;		z *= _z;		w *= _w;		return *this; }
	IC	SelfRef	mul		(SelfCRef v)					{ x *= v.x;			y *= v.y;		z *= v.z;		w *= v.w;		return *this; }
	IC  SelfRef	mul		(T s)							{ x *= s;			y *= s;			z *= s;			w *= s;			return *this; }
	IC	SelfRef	mul		(SelfCRef a, SelfCRef v)		{ x  = a.x*v.x;		y  = a.y*v.y;	z  = a.z*v.z;	w  = a.w*v.w;	return *this; }
	IC  SelfRef	mul		(SelfCRef a, T s)				{ x  = a.x*s;		y  = a.y*s;		z  = a.z*s;		w  = a.w*s;		return *this; }

	IC	SelfRef	div		(SelfCRef v)					{ x /= v.x;			y /= v.y;		z /= v.z;		w /= v.w;		return *this; }
	IC  SelfRef	div		(T s)							{ x /= s;			y /= s;			z /= s;			w /= s;			return *this; }
	IC	SelfRef	div		(SelfCRef a, SelfCRef v)		{ x  = a.x / v.x;	y  = a.y / v.y;	z  = a.z / v.z;	w  = a.w / v.w;	return *this; }
	IC  SelfRef	div		(SelfCRef a, T s)				{ x  = a.x / s;		y  = a.y / s;	z  = a.z / s;	w  = a.w / s;	return *this; }

	IC	SelfRef	min		(SelfCRef v)					{ x = std::min(x, v.x);		y = std::min(y, v.y);		z = std::min(z, v.z);		w = std::min(w, v.w);		return *this; }
	IC	SelfRef	min		(SelfCRef v1, SelfCRef v2)		{ x = std::min(v1.x, v2.x);	y = std::min(v1.y, v2.y);	z = std::min(v1.z, v2.z);	w = std::min(v1.w, v2.w);	return *this; }

	IC	SelfRef	max		(SelfCRef v)					{ x = std::max(x, v.x);		y = std::max(y, v.y);		z = std::max(z, v.z);		w = std::max(w, v.w);		return *this; }
	IC	SelfRef	max		(SelfCRef v1, SelfCRef v2)		{ x = std::max(v1.x, v2.x);	y = std::max(v1.y, v2.y);	z = std::max(v1.z, v2.z);	w = std::max(v1.w, v2.w);	return *this; }

	IC	SelfRef	abs		()								{ x =_abs(x);			y = _abs(y);		 z = _abs(z);		  w = _abs(w);			return *this; }
	IC	SelfRef	abs		(SelfCRef v)					{ x =_abs(v.x);			y = _abs(v.y);		 z = _abs(v.z);		  w = _abs(v.w);		return *this; }

	IC	SelfRef	ceil	()								{ x = std::ceil(x);		y = std::ceil(y);	 z = std::ceil(z);	  w = std::ceil(w);		return *this; }
	IC	SelfRef	ceil	(SelfCRef v)					{ x = std::ceil(v.x);	y = std::ceil(v.y);	 z = std::ceil(v.z);  w = std::ceil(v.w);	return *this; }

	IC	SelfRef	floor	()								{ x = std::floor(x);	y = std::floor(y);   z = std::floor(z);	  w = std::floor(w);	return *this; }
	IC	SelfRef	floor	(SelfCRef v)					{ x = std::floor(v.x);	y = std::floor(v.y); z = std::floor(v.z); w = std::floor(v.w);	return *this; }

	IC	BOOL	similar	(SelfCRef v, T E = EPS_L)		{ return _abs(x - v.x) < E && _abs(y - v.y) < E && _abs(z - v.z) < E && _abs(w - v.w) < E; }

	IC	T		magnitude_sqr()							{ return x*x + y*y + z*z + w*w;						}
	IC	T		magnitude()								{ return _sqrt(magnitude_sqr());					}
	IC	SelfRef	normalize()								{ return mul(T(1) / magnitude());					}
	IC	SelfRef	normalize_as_plane()					{ return mul(T(1) / _sqrt(x*x + y * y + z * z));	}

	// Linear interpolation
	IC	SelfRef	lerp(SelfCRef p1, SelfCRef p2, T t)
	{
		T invt = T(1) - t;
		x = p1.x*invt + p2.x*t;
		y = p1.y*invt + p2.y*t;
		z = p1.z*invt + p2.z*t;
		w = p1.w*invt + p2.w*t;
		return *this;	
	}

	// Clamp vector4
	IC	SelfRef	clamp	(SelfCRef min, const Self &max)
	{
		::clamp(x, min.x, max.x);
		::clamp(y, min.y, max.y);
		::clamp(z, min.z, max.z);
		::clamp(w, min.w, max.w);
		return *this;	
	}
	IC	SelfRef	clamp	(SelfCRef _v)
	{
		Self v;	v.abs(_v);
		::clamp(x, -v.x, v.x);
		::clamp(y, -v.y, v.y);
		::clamp(z, -v.z, v.z);
		::clamp(w, -v.w, v.w);
		return *this;	
	}

	// Average
	IC	SelfRef	average(SelfCRef p)
	{
		x = (x + p.x)*T(0.5);
		y = (y + p.y)*T(0.5);
		z = (z + p.z)*T(0.5);
		w = (w + p.w)*T(0.5);
		return *this;
	}
	IC	SelfRef	average(SelfCRef p1, SelfCRef p2)
	{
		x = (p1.x + p2.x)*T(0.5);
		y = (p1.y + p2.y)*T(0.5);
		z = (p1.z + p2.z)*T(0.5);
		w = (p1.w + p2.w)*T(0.5);
		return *this;
	}

	// Direct vector4 from point P by dir D with length M
	IC	SelfRef	mad(SelfCRef d, const T m)
	{
		x += d.x*m;
		y += d.y*m;
		z += d.z*m;
		w += d.w*m;
		return *this;
	}
	IC	SelfRef	mad(SelfCRef p, SelfCRef d, const T m)
	{
		x = p.x + d.x*m;
		y = p.y + d.y*m;
		z = p.z + d.z*m;
		w = p.w + d.w*m;
		return *this;
	}
	IC	SelfRef	mad(SelfCRef d, SelfCRef s)
	{
		x += d.x*s.x;
		y += d.y*s.y;
		z += d.z*s.z;
		w += d.w*s.w;
		return *this;
	}
	IC	SelfRef	mad(SelfCRef p, SelfCRef d, SelfCRef &s)
	{
		x = p.x + d.x*s.x;
		y = p.y + d.y*s.y;
		z = p.z + d.z*s.z;
		w = p.w + d.w*s.w;
		return *this;
	}

	// DotProduct
	IC	T		dotproduct(SelfCRef v) const
	{
		return x*v.x + y*v.y + z*v.z + w*v.w;
	}

	// CrossProduct
	ICF	SelfRef	crossproduct(SelfCRef v1, SelfCRef v2) // (v1,v2) -> this
	{
		x = v1.y*v2.z - v1.z*v2.y;
		y = v1.z*v2.x - v1.x*v2.z;
		z = v1.x*v2.y - v1.y*v2.x;
		w = v1.w*v2.w - v1.w*v2.w;
		return *this;
	}
};

using Fvector4 = _vector4<float>;
using Dvector4 = _vector4<double>;
using Ivector4 = _vector4<s32>;
#ifndef __BORLANDC__
	using Fvector4a = __declspec(align(16))	_vector4<float>;
	using Dvector4a = __declspec(align(16))	_vector4<double>;
	using Ivector4a = __declspec(align(16))	_vector4<s32>;
#endif

template <class T>
BOOL	_valid			(const _vector4<T> &v)	{ return _valid((T)v.x) && _valid((T)v.y) && _valid((T)v.z) && _valid((T)v.w); }
