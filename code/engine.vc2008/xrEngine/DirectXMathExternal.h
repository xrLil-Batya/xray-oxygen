#pragma once

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

// half_fov-angle-tangent
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

inline void BuildProj(float fFOV, float fAspect, float fNearPlane, float fFarPlane, DirectX::XMMATRIX &Matrics)
{
	return build_projection_HAT(tanf(fFOV / 2.f), fAspect, fNearPlane, fFarPlane, Matrics);
}

inline void BuildProjOrtho(float w, float h, float zn, float zf, DirectX::XMMATRIX &Matrics)
{
	Matrics = DirectX::XMMATRIX(
		2.f / w, 0, 0, 0,
		0, 2.f / h, 0, 0,
		0, 0, 1.f / (zf - zn), 0,
		0, 0, zn / (zn - zf), 1.f);
}

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

inline void BuildXForm(Fbox &B, DirectX::XMMATRIX &m)
{
	Fmatrix GSCMatrix = CastToGSCMatrix(m);

	B.xform(GSCMatrix);
}

inline void TransformVectorsByMatrix(const DirectX::XMMATRIX &m, Fvector &dest, const Fvector &v)
{
	float iw = 1.f / (v.x*m.r[0].m128_f32[3] + v.y*m.r[1].m128_f32[3] + v.z*m.r[2].m128_f32[3] + m.r[3].m128_f32[3]);

	dest.x = (v.x* m.r[0].m128_f32[0] + v.y* m.r[1].m128_f32[0] + v.z* m.r[2].m128_f32[0] + m.r[3].m128_f32[0])*iw;
	dest.y = (v.x* m.r[0].m128_f32[1] + v.y* m.r[1].m128_f32[1] + v.z* m.r[2].m128_f32[1] + m.r[3].m128_f32[1])*iw;
	dest.z = (v.x* m.r[0].m128_f32[2] + v.y* m.r[1].m128_f32[2] + v.z* m.r[2].m128_f32[2] + m.r[3].m128_f32[2])*iw;
}