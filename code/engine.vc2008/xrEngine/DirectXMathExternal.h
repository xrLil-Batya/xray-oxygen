#pragma once

/***************************************************************************
 *   Copyright (C) 2018 - ForserX & Oxydev
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/




/// <summary>Generate new cam direction </summary>
inline void BuildCamDir(const Fvector &vFrom, const Fvector &vView, const Fvector &vWorldUp, DirectX::XMMATRIX &Matrics)
{
	// Get the dot product, and calculate the projection of the z basis
	// vector3 onto the up vector3. The projection is the y basis vector3.
	float fDotProduct = vWorldUp.dotproduct(vView);

	Fvector vUp;
	vUp.mul(vView, -fDotProduct).add(vWorldUp).normalize();

	// The x basis vector3 is found simply with the cross product of the y
	// and z basis vectors
	Fvector vRight;
	vRight.crossproduct(vUp, vView);

	// Start building the Device.mView. The first three rows contains the basis
	// vectors used to rotate the view to point at the lookat point
	Matrics = DirectX::XMMATRIX(
		vRight.x, vUp.x, vView.x, 0.0f,
		vRight.y, vUp.y, vView.y, 0.0f,
		vRight.z, vUp.z, vView.z, 0.0f,
		-vFrom.dotproduct(vRight), -vFrom.dotproduct(vUp), -vFrom.dotproduct(vView), 1.f);
}

/// <summary>Half fov <-> angle <-> tangent</summary>
inline void build_projection_HAT(float HAT, float fAspect, float fNearPlane, float fFarPlane, DirectX::XMMATRIX &Matrics)
{
	float cot = 1.f / HAT;
	float w = fAspect * cot;
	float h = 1.f * cot;
	float Q = fFarPlane / (fFarPlane - fNearPlane);

	Matrics = DirectX::XMMATRIX(
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, Q, 1.f,
		0, 0, -Q * fNearPlane, 0);
}

/// <summary>Generate new a Matrix Projection</summary>
inline void BuildProj(float fFOV, float fAspect, float fNearPlane, float fFarPlane, DirectX::XMMATRIX &Matrics)
{
	return build_projection_HAT(tanf(fFOV / 2.f), fAspect, fNearPlane, fFarPlane, Matrics);
}

/// <summary>Generate new ortho projection to matrix </summary>
inline void BuildProjOrtho(float w, float h, float zn, float zf, DirectX::XMMATRIX &Matrics)
{
	Matrics = DirectX::XMMATRIX(
		2.f / w, 0, 0, 0,
		0, 2.f / h, 0, 0,
		0, 0, 1.f / (zf - zn), 0,
		0, 0, zn / (zn - zf), 1.f);
}

/// <summary>Convert DirectX::XMMATRIX to Fmatrix</summary>
inline Fmatrix CastToGSCMatrix(const DirectX::XMMATRIX &m)
{
	return
	{
		m.r[0].m128_f32[0], m.r[0].m128_f32[1], m.r[0].m128_f32[2], m.r[0].m128_f32[3],
		m.r[1].m128_f32[0], m.r[1].m128_f32[1], m.r[1].m128_f32[2], m.r[1].m128_f32[3],
		m.r[2].m128_f32[0], m.r[2].m128_f32[1], m.r[2].m128_f32[2], m.r[2].m128_f32[3],
		m.r[3].m128_f32[0], m.r[3].m128_f32[1], m.r[3].m128_f32[2], m.r[3].m128_f32[3]
	};
}

/// <summary>Call Fbox::xform for DirectX::XMMATRIX</summary>
inline void BuildXForm(Fbox &B, DirectX::XMMATRIX &m)
{
	Fmatrix GSCMatrix = CastToGSCMatrix(m);

	B.xform(GSCMatrix);
}

/// <summary>GSC Transform func for DirectX::XMMATRIX</summary>
inline void TransformVectorsByMatrix(const DirectX::XMMATRIX &m, Fvector &dest, const Fvector &v)
{
	float iw = 1.f / (v.x*m.r[0].m128_f32[3] + v.y*m.r[1].m128_f32[3] + v.z*m.r[2].m128_f32[3] + m.r[3].m128_f32[3]);

	dest.x = (v.x* m.r[0].m128_f32[0] + v.y* m.r[1].m128_f32[0] + v.z* m.r[2].m128_f32[0] + m.r[3].m128_f32[0])*iw;
	dest.y = (v.x* m.r[0].m128_f32[1] + v.y* m.r[1].m128_f32[1] + v.z* m.r[2].m128_f32[1] + m.r[3].m128_f32[1])*iw;
	dest.z = (v.x* m.r[0].m128_f32[2] + v.y* m.r[1].m128_f32[2] + v.z* m.r[2].m128_f32[2] + m.r[3].m128_f32[2])*iw;
}

/// <summary>GSC Transform func for DirectX::XMMATRIX</summary>
inline void TransformVector4ByMatrix(const DirectX::XMMATRIX &m, Fvector4 &dest, const Fvector &v)
{
	dest.w = v.x*m.r[0].m128_f32[3] + v.y*m.r[1].m128_f32[3] + v.z*m.r[2].m128_f32[3] + m.r[3].m128_f32[3];
	dest.x = (v.x*m.r[0].m128_f32[0] + v.y*m.r[1].m128_f32[0] + v.z*m.r[2].m128_f32[0] + m.r[3].m128_f32[0]) / dest.w;
	dest.y = (v.x*m.r[0].m128_f32[1] + v.y*m.r[1].m128_f32[1] + v.z*m.r[2].m128_f32[1] + m.r[3].m128_f32[1]) / dest.w;
	dest.z = (v.x*m.r[0].m128_f32[2] + v.y*m.r[1].m128_f32[2] + v.z*m.r[2].m128_f32[2] + m.r[3].m128_f32[2]) / dest.w;
}

/// <summary>GSC TransformDir func for DirectX::XMMATRIX</summary>
inline void TransformDirByMatrix(const DirectX::XMMATRIX &m, Fvector &dest, const Fvector &v)
{
	dest.x = v.x*m.r[0].m128_f32[0] + v.y*m.r[1].m128_f32[0] + v.z*m.r[2].m128_f32[0];
	dest.y = v.x*m.r[0].m128_f32[1] + v.y*m.r[1].m128_f32[1] + v.z*m.r[2].m128_f32[1];
	dest.z = v.x*m.r[0].m128_f32[2] + v.y*m.r[1].m128_f32[2] + v.z*m.r[2].m128_f32[2];
}

/// <summary>GSC TransformTiny23 func for DirectX::XMMATRIX</summary>
inline void TransformTiny23ByMatrix(const DirectX::XMMATRIX &m, Fvector &dest, const Fvector2 &v)
{
	dest.x = v.x*m.r[0].m128_f32[0] + v.y*m.r[1].m128_f32[0] + m.r[3].m128_f32[0];
	dest.y = v.x*m.r[0].m128_f32[1] + v.y*m.r[1].m128_f32[1] + m.r[3].m128_f32[1];
	dest.z = v.x*m.r[0].m128_f32[2] + v.y*m.r[1].m128_f32[2] + m.r[3].m128_f32[2];
}

/// <summary>GSC TransformTiny32 func for DirectX::XMMATRIX</summary>
inline void TransformTiny32ByMatrix(const DirectX::XMMATRIX &m, Fvector2 &dest, const Fvector &v)
{
	dest.x = v.x*m.r[0].m128_f32[0] + v.y*m.r[1].m128_f32[0] + v.z*m.r[2].m128_f32[0] + m.r[3].m128_f32[0];
	dest.y = v.x*m.r[0].m128_f32[1] + v.y*m.r[1].m128_f32[1] + v.z*m.r[2].m128_f32[1] + m.r[3].m128_f32[1];
}

/// <summary>GSC Inertion func for DirectX::XMMATRIX</summary>
inline void InertionByMatrix(DirectX::XMMATRIX &m, DirectX::XMMATRIX &m1, const float &v)
{
	float iv = 1.f - v;
	for (int i = 0; i < 4; ++i)
	{
		m.r[i].m128_f32[0] = m.r[i].m128_f32[0] * v + m1.r[i].m128_f32[0] * iv;
		m.r[i].m128_f32[1] = m.r[i].m128_f32[1] * v + m1.r[i].m128_f32[1] * iv;
		m.r[i].m128_f32[2] = m.r[i].m128_f32[2] * v + m1.r[i].m128_f32[2] * iv;
		m.r[i].m128_f32[3] = m.r[i].m128_f32[3] * v + m1.r[i].m128_f32[3] * iv;
	}
}

inline void InertionByVector(DirectX::XMVECTOR &origin, const DirectX::XMVECTOR &g, const float &v)
{
	float inv = 1.0f - v;
	origin.
}


/// <summary>Set i vector func for DirectX::XMMATRIX</summary>
inline void Set_i(DirectX::XMMATRIX &m, const float &x, const float &y, const float &z, const float &w = 0)
{
	m.r[0].m128_f32[0] = x;
	m.r[0].m128_f32[1] = y;
	m.r[0].m128_f32[2] = z;
	m.r[0].m128_f32[3] = w;
}

/// <summary>Set j vector func for DirectX::XMMATRIX</summary>
inline void Set_j(DirectX::XMMATRIX &m, const float &x, const float &y, const float &z, const float &w = 0)
{
	m.r[1].m128_f32[0] = x;
	m.r[1].m128_f32[1] = y;
	m.r[1].m128_f32[2] = z;
	m.r[1].m128_f32[3] = w;
}

/// <summary>Set z vector func for DirectX::XMMATRIX</summary>
inline void Set_z(DirectX::XMMATRIX &m, const float &x, const float &y, const float &z, const float &w = 0)
{
	m.r[2].m128_f32[0] = x;
	m.r[2].m128_f32[1] = y;
	m.r[2].m128_f32[2] = z;
	m.r[2].m128_f32[3] = w;
}

/// <summary>Set c vector func for DirectX::XMMATRIX (vector c is 4 vector in matrix (e. g. _14 _24 _34 _44)</summary>
inline void Set_c(DirectX::XMMATRIX &m, const float &x, const float &y, const float &z, const float &w = 0)
{
	m.r[3].m128_f32[0] = x;
	m.r[3].m128_f32[1] = y;
	m.r[3].m128_f32[2] = z;
	m.r[3].m128_f32[3] = w;
}

/// <summary>Set i vector is some value func for DirectX::XMMATRIX</summary>
inline void Set_r0_w(DirectX::XMMATRIX &m, const float &w = 0)
{
	m.r[0] = Get_Zero(w);
}

/// <summary>Set j vector to 0 but w is some value func for DirectX::XMMATRIX</summary>
inline void Set_r1_w(DirectX::XMMATRIX &m, const float &w = 0)
{
	m.r[1] = Get_Zero(w);
}

/// <summary>Set z vector to 0 but w is some value func for DirectX::XMMATRIX</summary>
inline void Set_r2_w(DirectX::XMMATRIX &m, const float &w = 0)
{
	m.r[2] = Get_Zero(w);
}

/// <summary>Set c vector to 0 but w is some value func for DirectX::XMMATRIX</summary>
inline void Set_r3_w(DirectX::XMMATRIX &m, const float &w = 0)
{
	m.r[3] = Get_Zero(w);
}

/// <summary>Returns Zero Vector func for DirectX::XMMATRIX</summary>
inline DirectX::XMVECTOR GetZeroVector(const float &w = 0)
{
	DirectX::XMVECTOR F = DirectX::XMVectorZero();
	F.m128_f32[3] = w;
	return F;
}
 


/* Hot tip delete this after implemented all methods
	_11 - m.r[0].m128_f32[0]
	_12 - m.r[0].m128_f32[1]
	_13 - m.r[0].m128_f32[2]
	_14 - m.r[0].m128_f32[3]

	_21 - m.r[1].m128_f32[0]
	_22 - m.r[1].m128_f32[1]
	_23 - m.r[1].m128_f32[2]
	_24 - m.r[1].m128_f32[3]

	_31 - m.r[2].m128_f32[0]
	_32 - m.r[2].m128_f32[1]
	_33 - m.r[2].m128_f32[2]
	_34 - m.r[2].m128_f32[3]

	_41 - m.r[3].m128_f32[0]
	_42 - m.r[3].m128_f32[1]
	_43 - m.r[3].m128_f32[2]
	_44 - m.r[3].m128_f32[3]
*/