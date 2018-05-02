#ifndef	DOF_H_INCLUDED
#define	DOF_H_INCLUDED

//#define USE_DOF

#ifndef	USE_DOF

half3	dof(float2 center)
{
	half3 	img 	= tex2D		(s_image, center);
	return	img;
}

#else	//	USE_DOF

// x - near y - focus z - far w - sky distance
half4	dof_params;
half3	dof_kernel;	// x,y - resolution pre-scaled z - just kernel size

half DOFFactor( half depth)
{
	half	dist_to_focus	= depth-dof_params.y;
	half 	blur_far	= saturate( dist_to_focus
					/ (dof_params.z-dof_params.y) );
	half 	blur_near	= saturate( dist_to_focus
					/ (dof_params.x-dof_params.y) );
	half 	blur 		= blur_near+blur_far;
	blur*=blur;
	return blur;
}


#include "blur_bokeh.h"
#define MAXCOF		7.h
#define EPSDEPTH	0.0001h
half3	dof(float2 center)
{
	// Scale tap offsets based on render target size
	half 	depth		= tex2D(s_position,center).z;
	if (depth <= EPSDEPTH)	depth = dof_params.w;
	half	blur 		= DOFFactor(depth);

    half3 final = bokeh_dof(center, blur);

	return half4(final, 1.0);
}


#endif	//	USE_DOF

#endif	//	DOF_H_INCLUDED