#pragma once

#define UNIT_TOLERANCE 0.001f
// Quaternion magnitude must be closer than this tolerance to 1.0 to be
// considered a unit quaternion

#define QZERO_TOLERANCE 0.00001f
// quaternion magnitude must be farther from this tolerance to 0.0 to be
// normalized

#define TRACE_QZERO_TOLERANCE 0.1f
// trace of matrix must be greater than this to be used for converting a matrix
// to a quaternion.

#define AA_QZERO_TOLERANCE 0.0001f
#define QEPSILON 0.00001f

template <class T>
struct _quaternion {
    typedef T TYPE;
    typedef _quaternion<T> Self;
    typedef Self& SelfRef;
    typedef const Self& SelfCRef;

private:
    static T _asin_(T x)
    {
        const T c1 = 0.892399f;
        const T c3 = 1.693204f;
        const T c5 = -3.853735f;
        const T c7 = 2.838933f;

        const T x2 = x * x;
        const T d = x * (c1 + x2 * (c3 + x2 * (c5 + x2 * c7)));

        return d;
    }

    static T _acos_(T x)
    {
        return PI_DIV_2 - _asin_(x);
    }

public:
    T x, y, z, w;

    IC SelfRef set(T W, T X, T Y, T Z) // don't normalize
    {
        x = X;
        y = Y;
        z = Z;
        w = W;
        return *this;
    }

    IC SelfRef set(SelfCRef Q) // don't normalize
    {
        set(Q.w, Q.x, Q.y, Q.z);
        return *this;
    }

    SelfRef set(const _matrix<T>& m);

    // multiplies q1 * q2, and places the result in *this.
    // no failure. 	renormalization not automatic

    /*
	q1*q2 = q3 =
		(w1*w2 - x1*x2 - y1*y2 - z1*z2)     {w3}
		(w1*x2 + x1*w2 + y1*z2 - z1*y2)i	{x3}
		(w1*y2 - x1*z2 + y1*w2 + z1*x2)j    {y3}
		(w1*z2 + x1*y2 - y1*x2 + z1*w2)k	{z3}
*/
    SelfRef mul(SelfCRef q1l, SelfCRef q2l)
    {
        VERIFY(q1l.isValid());
        VERIFY(q2l.isValid());

        w = ((q1l.w * q2l.w) - (q1l.x * q2l.x)
            - (q1l.y * q2l.y) - (q1l.z * q2l.z));

        x = ((q1l.w * q2l.x) + (q1l.x * q2l.w)
            + (q1l.y * q2l.z) - (q1l.z * q2l.y));

        y = ((q1l.w * q2l.y) - (q1l.x * q2l.z)
            + (q1l.y * q2l.w) + (q1l.z * q2l.x));

        z = ((q1l.w * q2l.z) + (q1l.x * q2l.y)
            - (q1l.y * q2l.x) + (q1l.z * q2l.w));
        return *this;
    }

    SelfRef add(SelfCRef q1, SelfCRef q2)
    {
        x = q1.x + q2.x;
        y = q1.y + q2.y;
        z = q1.z + q2.z;
        w = q1.w + q2.w;
        return *this;
    }

    SelfRef sub(SelfCRef q1, SelfCRef q2)
    {
        x = q1.x - q2.x;
        y = q1.y - q2.y;
        z = q1.z - q2.z;
        w = q1.w - q2.w;
        return *this;
    }

    SelfRef add(SelfCRef q)
    {
        x += q.x;
        y += q.y;
        z += q.z;
        w += q.w;
        return *this;
    }

    SelfRef sub(SelfCRef q)
    {
        x -= q.x;
        y -= q.y;
        z -= q.z;
        w -= q.w;
        return *this;
    }

    // validates numerical stability
    IC bool isValid() const
    {
        if ((w * w) < 0.0f)
            return false;
        if ((x * x) < 0.0f)
            return false;
        if ((y * y) < 0.0f)
            return false;
        if ((z * z) < 0.0f)
            return false;
        return true;
    }

    // checks for Unit-length quanternion
    IC bool isUnit()
    {
        T m = magnitude();

        if ((m < 1.0 + UNIT_TOLERANCE) && (m > 1.0 - UNIT_TOLERANCE))
            return true;
        return false;
    }

    // normalizes Q to be a unit geQuaternion
    SelfRef normalize(void)
    {
        T m, one_over_magnitude;

        m = _sqrt(magnitude());

        if ((m < QZERO_TOLERANCE) && (m > -QZERO_TOLERANCE))
            return *this;

        one_over_magnitude = 1.0f / m;

        w *= one_over_magnitude;
        x *= one_over_magnitude;
        y *= one_over_magnitude;
        z *= one_over_magnitude;
        return *this;
    }

    // inversion
    ICF SelfRef inverse(SelfCRef Q)
    {
        return set(Q.w, -Q.x, -Q.y, -Q.z);
    }

    ICF SelfRef inverse()
    {
        return set(w, -x, -y, -z);
    }

    ICF SelfRef inverse_with_w(SelfCRef Q)
    {
        return set(-Q.w, -Q.x, -Q.y, -Q.z);
    }

    ICF SelfRef inverse_with_w()
    {
        return set(-w, -x, -y, -z);
    }

    // identity - no rotation
    ICF SelfRef identity(void)
    {
        return set(1.f, 0.f, 0.f, 0.f);
    }

    // square length
    T magnitude(void)
    {
        return w * w + x * x + y * y + z * z;
    }

    // makes unit rotation
    SelfRef rotationYawPitchRoll(T _x, T _y, T _z)
    {
        T fSinYaw = _sin(_x * .5f);
        T fCosYaw = _cos(_x * .5f);
        T fSinPitch = _sin(_y * .5f);
        T fCosPitch = _cos(_y * .5f);
        T fSinRoll = _sin(_z * .5f);
        T fCosRoll = _cos(_z * .5f);

        x = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
        y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
        z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
        w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;
        return *this;
    }

    // makes unit rotation
    ICF SelfRef rotationYawPitchRoll(const Fvector& ypr)
    {
        return rotationYawPitchRoll(ypr.x, ypr.y, ypr.z);
    }

    // set a quaternion from an axis and a rotation around the axis
    ICF SelfRef rotation(Fvector& axis, T angle)
    {
        T sinTheta;

        w = _cos(angle * 0.5f);
        sinTheta = _sin(angle * 0.5f);
        x = sinTheta * axis.x;
        y = sinTheta * axis.y;
        z = sinTheta * axis.z;
        return *this;
    }

    // gets an axis and angle of rotation around the axis from a quaternion
    // returns TRUE if there is an axis.
    // returns FALSE if there is no axis (and Axis is set to 0,0,0, and Theta is 0)

    bool get_axis_angle(_vector3<T>& axis, T& angle) const
    {
        T s = _sqrt(x * x + y * y + z * z);
        if (s > EPS_S) {
            T OneOverSinTheta = 1.f / s;
            axis.x = OneOverSinTheta * x;
            axis.y = OneOverSinTheta * y;
            axis.z = OneOverSinTheta * z;
            angle = 2.0f * std::atan2(s, w);
            return true;
        } else {
            axis.x = axis.y = axis.z = 0.0f;
            angle = 0.0f;
            return false;
        }
    }

    // spherical interpolation between q0 and q1.   0<=t<=1
    // resulting quaternion is 'between' q0 and q1
    // with t==0 being all q0, and t==1 being all q1.
    // returns a quaternion with a positive W - always takes shortest route
    // through the positive W domain.
    ICF SelfRef slerp(SelfCRef Q0, SelfCRef Q1, T tm)
    {
        T Scale0, Scale1, sign;

#ifdef DEBUG
        if (!((T(0) <= tm) && (tm <= T(1))))
            Debug.fatal(DEBUG_INFO, "Quaternion::slerp - invalid 'tm' arrived: %f", tm);
#endif

        T cosom = (Q0.w * Q1.w) + (Q0.x * Q1.x) + (Q0.y * Q1.y) + (Q0.z * Q1.z);

        if (cosom < 0) 
		{
            cosom = -cosom;
            sign = -1.f;
        } else  sign = 1.f;

        if ((1.0f - cosom) > EPS) {
            T omega = _acos_(cosom);
            T i_sinom = 1.f / std::sin(omega);
            T t_omega = tm * omega;
            Scale0 = std::sin(omega - t_omega) * i_sinom;
            Scale1 = std::sin(t_omega) * i_sinom;
        } else {
            // has numerical difficulties around cosom == 0
            // in this case degenerate to linear interpolation
            Scale0 = 1.0f - tm;
            Scale1 = tm;
        }
        Scale1 *= sign;

        x = Scale0 * Q0.x + Scale1 * Q1.x;
        y = Scale0 * Q0.y + Scale1 * Q1.y;
        z = Scale0 * Q0.z + Scale1 * Q1.z;
        w = Scale0 * Q0.w + Scale1 * Q1.w;
        return *this;
    }

    // return TRUE if quaternions differ elementwise by less than Tolerance.
    ICF bool cmp(SelfCRef Q, T Tolerance = 0.0001f)
    {
		return (((_abs(x + Q.x) <= Tolerance) && (_abs(y + Q.y) <= Tolerance) && (_abs(z + Q.z) <= Tolerance) && (_abs(w + Q.w) <= Tolerance))
            /* they are the same with same signs */
            || ((_abs(x - Q.x) <= Tolerance) && (_abs(y - Q.y) <= Tolerance) && (_abs(z - Q.z) <= Tolerance) && (_abs(w - Q.w) <= Tolerance))) 
		/* return */ ? true : false;
    }

    SelfRef ln(SelfCRef Q)
    {
        T n = Q.x * Q.x + Q.y * Q.y + Q.z * Q.z;
        T r = _sqrt(n);
        T t = (r > EPS_S) ? 
		std::atan2(r, Q.w) / r : T(0);
        x = t * Q.x;
        y = t * Q.y;
        z = t * Q.z;
        w = .5f * _log(n + Q.w * Q.w);
        return *this;
    }

    SelfRef exp(SelfCRef Q)
    {
        T r = _sqrt(Q.x * Q.x + Q.y * Q.y + Q.z * Q.z);
        T et = std::exp(Q.w);
        T s = (r >= EPS_S) ? et * std::sin(r) / r : 0.f;
        x = s * Q.x;
        y = s * Q.y;
        z = s * Q.z;
        w = et * _cos(r);
        return *this;
    }
};

typedef _quaternion<float> Fquaternion;
typedef _quaternion<double> Dquaternion;

template struct XRCORE_API _quaternion<float>;
template struct XRCORE_API _quaternion<double>;

template <class T>
BOOL _valid(const _quaternion<T>& s) { return _valid(s.x) && _valid(s.y) && _valid(s.z) && _valid(s.w); }

#undef UNIT_TOLERANCE
#undef QZERO_TOLERANCE
#undef TRACE_QZERO_TOLERANCE
#undef AA_QZERO_TOLERANCE
#undef QEPSILON