#ifndef	DOF_H_INCLUDED
#define	DOF_H_INCLUDED

//#define USE_DOF

#ifndef	USE_DOF

float3	dof(float2 center)
{
//	float3 	img 	= tex2D		(s_image, center);
	float3 	img 	= s_image.Sample( smp_rtlinear, center);
	return	img;
}

#else	//	USE_DOF

// x - near y - focus z - far w - sky distance
float4	dof_params;
float3	dof_kernel;	// x,y - resolution pre-scaled z - just kernel size

float DOFFactor( float depth)
{
	float	dist_to_focus	= depth-dof_params.y;
	float 	blur_far	= saturate( dist_to_focus
					/ (dof_params.z-dof_params.y) );
	float 	blur_near	= saturate( dist_to_focus
					/ (dof_params.x-dof_params.y) );
	float 	blur 		= blur_near+blur_far;
	blur*=blur;
	return blur;
}


#include "blur_bokeh.h"
#define MAXCOF		7.h
#define EPSDEPTH	0.0001h
float3	dof(float2 center)
{

#ifndef USE_MSAA
   float 	depth		= s_position.Sample( smp_nofilter, center).z;
#else
   float 	depth		= s_position.Load( int3( center * pos_decompression_params2.xy ,0),0 ).z;
#endif
	if (depth <= EPSDEPTH)	depth = dof_params.w;
	float	blur 		= DOFFactor(depth);

	float3 final = bokeh_dof(center, blur);
	
	return 	float4	(final, 1.0);
}


#endif	//	USE_DOF

#endif	//	DOF_H_INCLUDED