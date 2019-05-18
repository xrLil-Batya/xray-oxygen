#include "stdafx.h"
#include "_bitwise.h"

XRCORE_API float fFloorSSE2(const float &x)
{
	// copy to low DST element 
	__m128 f = _mm_set_ss(x);
	__m128 one = _mm_set_ss(1.0f);

	// convert XMM float->int->float for check on good number 
	__m128 t = _mm_cvtepi32_ps(_mm_cvttps_epi32(f));

	// t - (f < t ? 0xffffffff : 0 & full float value)
	__m128 r = _mm_sub_ps(t, _mm_and_ps(_mm_cmplt_ps(f, t), one));

	return _mm_cvtss_f32(r);
}
