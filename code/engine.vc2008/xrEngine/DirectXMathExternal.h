#pragma once
#include <DirectXMath.h>
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
namespace XRay
{
	namespace Math
	{
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
		};

		struct Matrix4x4
		{
			struct IntricsVect
			{
				DirectX::XMVECTOR Vect;

			public:

				DirectX::XMVECTOR operator()()
				{
					return Vect;
				};

				float& operator[](size_t id)
				{
					return Vect.m128_f32[id];
				}

				const float& operator[](size_t id) const 
				{
					return Vect.m128_f32[id];
				}

				void operator=(DirectX::XMVECTOR &VectObj)
				{
					Vect = VectObj;
				}

			};

		protected:
			/// <summary>Half fov <-> angle <-> tangent</summary>
			inline void build_projection_HAT(float HAT, float fAspect, float fNearPlane, float fFarPlane, DirectX::XMMATRIX &Matrics);

		public:
			Matrix4x4(DirectX::XMMATRIX a) 
			{ 
				Matrix = a;

				x = Matrix.r[0];
				y = Matrix.r[1];
				z = Matrix.r[2];
				w = Matrix.r[3];
			}

			Matrix4x4()
			{

				Matrix =
				{
					0, 0, 0, 0,
					0, 0, 0, 0,
					0, 0, 0, 0,
					0, 0, 0, 0
				};

				x = Matrix.r[0];
				y = Matrix.r[1];
				z = Matrix.r[2];
				w = Matrix.r[3];
			}

			Matrix4x4
			(
				float a1, float a2, float a3, float a4,
				float b1, float b2, float b3, float b4,
				float c1, float c2, float c3, float c4,
				float d1, float d2, float d3, float d4
			)
			{
				Matrix =
				{
					a1, a2, a3, a4,
					b1, b2, b3, b4,
					c1, c2, c3, c4,
					d1, d2, d3, d4
				};

				x = Matrix.r[0];
				y = Matrix.r[1];
				z = Matrix.r[2];
				w = Matrix.r[3];
			}

			/// <summary>Generate new cam direction </summary>
			inline void BuildCamDir(const Fvector &vFrom, const Fvector &vView, const Fvector &vWorldUp);
			/// <summary>Generate new a Matrix Projection</summary>
			inline void BuildProj(float fFOV, float fAspect, float fNearPlane, float fFarPlane);
			/// <summary>Generate new ortho projection to matrix </summary>
			inline void BuildProjOrtho(float w, float h, float zn, float zf);

			/// <summary>Multiplication matrix by matrix</summary>
			inline void Multiply(Matrix4x4 a, Matrix4x4 b) { Matrix = DirectX::XMMatrixMultiply(a, b); }

			/// <summary>Inversion matrix by matrix</summary>
			inline void InvertMatrixByMatrix(const DirectX::XMMATRIX &a);
			/// <summary>Call Fbox::xform for DirectX::XMMATRIX</summary>
			inline void BuildXForm(Fbox &B);

			inline void Translate(Fvector3 diff)
			{
				Matrix = DirectX::XMMatrixTranslation(diff.x, diff.y, diff.z);
			}

			inline void Inverse(DirectX::XMVECTOR* pDeterminant, const DirectX::XMMATRIX& refMatrix)
			{
				Matrix = DirectX::XMMatrixInverse(pDeterminant, refMatrix);
			}

		public:
			union
			{
				struct
				{
					IntricsVect x;
					IntricsVect y;
					IntricsVect z;
					IntricsVect w;
				};

				DirectX::XMMATRIX Matrix;
			};
						
		public:
			inline operator Fmatrix () { return CastToGSCMatrix(Matrix); }
			inline operator Fmatrix () const { return CastToGSCMatrix(Matrix); }
			inline operator DirectX::XMMATRIX() { return Matrix; }
			inline operator DirectX::XMMATRIX() const { return Matrix; }
			inline void operator= (const DirectX::XMMATRIX &a) { Matrix = a; }
			inline void operator= (const Matrix4x4 &a) { Matrix = a.Matrix; }
			inline void operator= (const Fmatrix &a)
			{
				Matrix =
				{
					a._11, a._12, a._13, a._14,
					a._21, a._22, a._23, a._24,
					a._31, a._32, a._33, a._34,
					a._41, a._42, a._43, a._44
				};
			}
		};

		inline void Matrix4x4::BuildCamDir(const Fvector &vFrom, const Fvector &vView, const Fvector &vWorldUp)
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
			Matrix = DirectX::XMMATRIX(
				vRight.x, vUp.x, vView.x, 0.0f,
				vRight.y, vUp.y, vView.y, 0.0f,
				vRight.z, vUp.z, vView.z, 0.0f,
				-vFrom.dotproduct(vRight), -vFrom.dotproduct(vUp), -vFrom.dotproduct(vView), 1.f);
		}

		inline void Matrix4x4::build_projection_HAT(float HAT, float fAspect, float fNearPlane, float fFarPlane, DirectX::XMMATRIX &Matrics)
		{
			float cot = 1.f / HAT;
			float W = fAspect * cot;
			float h = 1.f * cot;
			float Q = fFarPlane / (fFarPlane - fNearPlane);

			Matrics = DirectX::XMMATRIX(
				W, 0, 0, 0,
				0, h, 0, 0,
				0, 0, Q, 1.f,
				0, 0, -Q * fNearPlane, 0);
		}

		inline void Matrix4x4::BuildProj(float fFOV, float fAspect, float fNearPlane, float fFarPlane)
		{
			return build_projection_HAT(tanf(fFOV / 2.f), fAspect, fNearPlane, fFarPlane, Matrix);
		}

		inline void Matrix4x4::BuildProjOrtho(float W, float h, float zn, float zf)
		{
			Matrix = DirectX::XMMATRIX(
				2.f / W, 0, 0, 0,
				0, 2.f / h, 0, 0,
				0, 0, 1.f / (zf - zn), 0,
				0, 0, zn / (zn - zf), 1.f);
		}

		inline void Matrix4x4::BuildXForm(Fbox &B)
		{
			B.xform(CastToGSCMatrix(Matrix));
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

		/// <summary>GSC TransformDir func for DirectX::XMMATRIX</summary>
		inline void TransformDirByMatrix(const DirectX::XMMATRIX &m, Fvector &v)
		{
			Fvector res;
			TransformDirByMatrix(m, res, v);
			v.set(res);
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
			origin.m128_f32[0] = v * origin.m128_f32[0] + inv * g.m128_f32[0];
			origin.m128_f32[1] = v * origin.m128_f32[1] + inv * g.m128_f32[1];
			origin.m128_f32[2] = v * origin.m128_f32[2] + inv * g.m128_f32[2];
		}

		inline void InertionByVector(DirectX::XMVECTOR &origin, const Fvector &g, const float &v)
		{
			float inv = 1.0f - v;
			origin.m128_f32[0] = v * origin.m128_f32[0] + inv * g.x;
			origin.m128_f32[1] = v * origin.m128_f32[1] + inv * g.y;
			origin.m128_f32[2] = v * origin.m128_f32[2] + inv * g.z;
		}

		inline void ClampByVector(DirectX::XMVECTOR &origin, const Fvector &mn, const Fvector &mx)
		{
			::clamp<float>(origin.m128_f32[0], mn.x, mx.x);
			::clamp<float>(origin.m128_f32[1], mn.y, mx.y);
			::clamp<float>(origin.m128_f32[2], mn.z, mx.z);
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

		/// <summary>Returns Zero Vector func for DirectX::XMMATRIX</summary>
		inline DirectX::XMVECTOR GetZeroVector(const float &w = 0)
		{
			DirectX::XMVECTOR F = DirectX::XMVectorZero();
			F.m128_f32[3] = w;
			return F;
		}

		/// <summary>Set i vector is some value func for DirectX::XMMATRIX</summary>
		inline void Set_r0_w(DirectX::XMMATRIX &m, const float &w = 0)
		{
			m.r[0] = GetZeroVector(w);
		}

		/// <summary>Set j vector to 0 but w is some value func for DirectX::XMMATRIX</summary>
		inline void Set_r1_w(DirectX::XMMATRIX &m, const float &w = 0)
		{
			m.r[1] = GetZeroVector(w);
		}

		/// <summary>Set z vector to 0 but w is some value func for DirectX::XMMATRIX</summary>
		inline void Set_r2_w(DirectX::XMMATRIX &m, const float &w = 0)
		{
			m.r[2] = GetZeroVector(w);
		}

		/// <summary>Set c vector to 0 but w is some value func for DirectX::XMMATRIX</summary>
		inline void Set_r3_w(DirectX::XMMATRIX &m, const float &w = 0)
		{
			m.r[3] = GetZeroVector(w);
		}


		inline float XMV_square_magnitude(const DirectX::XMVECTOR &v)
		{
			return (v.m128_f32[0] * v.m128_f32[0] + v.m128_f32[1] * v.m128_f32[1] + v.m128_f32[2] * v.m128_f32[2]);
		}

		inline float XMV_magnitude(const DirectX::XMVECTOR &v)
		{
			return (::_sqrt(XMV_square_magnitude(v)));
		}
		
		inline float XMFloat2Len(const DirectX::XMFLOAT2& Flt2)
		{
			return sqrtf(Flt2.x * Flt2.x + Flt2.y * Flt2.y);
		}

		inline void Matrix4x4::InvertMatrixByMatrix(const DirectX::XMMATRIX &a)
		{
			// faster than self-invert
			float fDetInv = (a.r[0].m128_f32[0] * (a.r[1].m128_f32[1] * a.r[2].m128_f32[2] - a.r[1].m128_f32[2] * a.r[2].m128_f32[1]) -
				a.r[0].m128_f32[1] * (a.r[1].m128_f32[0] * a.r[2].m128_f32[2] - a.r[1].m128_f32[2] * a.r[2].m128_f32[0]) +
				a.r[0].m128_f32[2] * (a.r[1].m128_f32[0] * a.r[2].m128_f32[1] - a.r[1].m128_f32[1] * a.r[2].m128_f32[0]));

			VERIFY(_abs(fDetInv) > flt_zero);
			fDetInv = 1.0f / fDetInv;

			Matrix.r[0].m128_f32[0] = fDetInv * (a.r[1].m128_f32[1] * a.r[2].m128_f32[2] - a.r[1].m128_f32[2] * a.r[2].m128_f32[1]);
			Matrix.r[0].m128_f32[1] = -fDetInv * (a.r[0].m128_f32[1] * a.r[2].m128_f32[2] - a.r[0].m128_f32[2] * a.r[2].m128_f32[1]);
			Matrix.r[0].m128_f32[2] = fDetInv * (a.r[0].m128_f32[1] * a.r[1].m128_f32[2] - a.r[0].m128_f32[2] * a.r[1].m128_f32[1]);
			Matrix.r[0].m128_f32[3] = 0.0f;
			
			Matrix.r[1].m128_f32[0] = -fDetInv * (a.r[1].m128_f32[0] * a.r[2].m128_f32[2] - a.r[1].m128_f32[2] * a.r[2].m128_f32[0]);
			Matrix.r[1].m128_f32[1] = fDetInv * (a.r[0].m128_f32[0] * a.r[2].m128_f32[2] - a.r[0].m128_f32[2] * a.r[2].m128_f32[0]);
			Matrix.r[1].m128_f32[2] = -fDetInv * (a.r[0].m128_f32[0] * a.r[1].m128_f32[2] - a.r[0].m128_f32[2] * a.r[1].m128_f32[0]);
			Matrix.r[1].m128_f32[3] = 0.0f;
			
			Matrix.r[2].m128_f32[0] = fDetInv * (a.r[1].m128_f32[0] * a.r[2].m128_f32[1] - a.r[1].m128_f32[1] * a.r[2].m128_f32[0]);
			Matrix.r[2].m128_f32[1] = -fDetInv * (a.r[0].m128_f32[0] * a.r[2].m128_f32[1] - a.r[0].m128_f32[1] * a.r[2].m128_f32[0]);
			Matrix.r[2].m128_f32[2] = fDetInv * (a.r[0].m128_f32[0] * a.r[1].m128_f32[1] - a.r[0].m128_f32[1] * a.r[1].m128_f32[0]);
			Matrix.r[2].m128_f32[3] = 0.0f;
			
			Matrix.r[3].m128_f32[0] = -(a.r[3].m128_f32[0] * Matrix.r[0].m128_f32[0] + a.r[3].m128_f32[1] * Matrix.r[1].m128_f32[0] + a.r[3].m128_f32[2] * Matrix.r[2].m128_f32[0]);
			Matrix.r[3].m128_f32[1] = -(a.r[3].m128_f32[0] * Matrix.r[0].m128_f32[1] + a.r[3].m128_f32[1] * Matrix.r[1].m128_f32[1] + a.r[3].m128_f32[2] * Matrix.r[2].m128_f32[1]);
			Matrix.r[3].m128_f32[2] = -(a.r[3].m128_f32[0] * Matrix.r[0].m128_f32[2] + a.r[3].m128_f32[1] * Matrix.r[1].m128_f32[2] + a.r[3].m128_f32[2] * Matrix.r[2].m128_f32[2]);
			Matrix.r[3].m128_f32[3] = 1.0f;
		}
	}
};

using Matrix4x4 = XRay::Math::Matrix4x4;