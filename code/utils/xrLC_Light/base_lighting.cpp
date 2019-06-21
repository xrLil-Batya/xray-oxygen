#include "stdafx.h"

#include "base_lighting.h"
#include "serialize.h"
#include "xrHardwareLight.h"

void base_lighting::select(xr_vector<R_Light>& dest, xr_vector<R_Light>& src, Fvector& P, float R)
{
	Fsphere Sphere;
	Sphere.set(P, R);
	dest.clear();

	for (R_Light refLight : src)
	{
		if (refLight.type == LT_POINT)
		{
			float dist = Sphere.P.distance_to(refLight.position);
			if (dist > (Sphere.R + refLight.range))	continue;
		}
		dest.push_back(refLight);
	}
}

void base_lighting::select(base_lighting& from, Fvector& P, float R)
{
    if (xrHardwareLight::IsEnabled())
    {
        select(rgbIndexes, from.rgb, P, R);
        select(hemiIndexes, from.rgb, P, R);
        select(sunIndexes, from.rgb, P, R);
    }
    else
    {
        select(rgb, from.rgb, P, R);
        select(hemi, from.hemi, P, R);
        select(sun, from.sun, P, R);
    }
}

void base_lighting::select(xr_vector<DWORD>& dest, xr_vector<R_Light>& src, Fvector& P, float R)
{
	Fsphere		Sphere;
	Sphere.set(P, R);
	dest.clear();

	u32 IteratorForLight = 0;

	for (R_Light& LightObj : src)
	{
		if (LightObj.type == LT_POINT) 
		{
			float dist = Sphere.P.distance_to(LightObj.position);
			if (dist > (Sphere.R + LightObj.range))	continue;
		}

		dest.push_back(IteratorForLight);
		IteratorForLight++;
	}
}

/*
xr_vector<R_Light>		rgb;		// P,N
xr_vector<R_Light>		hemi;		// P,N
xr_vector<R_Light>		sun;		// P
*/
void base_lighting::read(IReader &r)
{
	r_pod_vector(r, rgb);
	r_pod_vector(r, hemi);
	r_pod_vector(r, sun);
}

void base_lighting::write(IWriter &w) const
{
	w_pod_vector(w, rgb);
	w_pod_vector(w, hemi);
	w_pod_vector(w, sun);
}