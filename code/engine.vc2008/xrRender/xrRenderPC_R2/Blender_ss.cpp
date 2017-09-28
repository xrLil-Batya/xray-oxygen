#include "stdafx.h"
#pragma hdrstop

#include "Blender_ss.h"

CBlender_ss::CBlender_ss	()	{	description.CLS		= 0;	}
CBlender_ss::~CBlender_ss	()	{	}
 
void	CBlender_ss::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	
	switch (C.iElement)
	{
		case 0:
			C.r_Pass		("null",			"SunShaftsMask",	FALSE,	FALSE,	FALSE);
			C.r_Sampler_rtf	("sPosition",		r2_RT_P);
			C.r_Sampler_clf	("sScene",			r2_RT_generic0);
			C.r_End			();
			break;
		case 1:
			C.r_Pass		("null",			"SunShaftsMaskBlur",	FALSE,	FALSE,	FALSE);
			C.r_Sampler_clf	("sMask",			r2_RT_SunShaftsMask);
			C.r_End			();
			break;
		case 2:
			C.r_Pass		("null",			"SunShaftsGeneration",	FALSE,	FALSE,	FALSE);
			C.r_Sampler_clf	("sMaskBlur",		r2_RT_SunShaftsMaskSmoothed);
			C.r_End			();
			break;
		case 3:
			C.r_Pass		("null",			"SunShaftsGeneration",	FALSE,	FALSE,	FALSE);
			C.r_Sampler_clf	("sMaskBlur",		r2_RT_SunShaftsPass0);
			C.r_End			();
			break;
		//case 4:
			//C.r_Pass		("null",			"SunShaftsGeneration",	FALSE,	FALSE,	FALSE);
			//C.r_Sampler_clf	("sMaskBlur",		r2_RT_SunShaftsMaskSmoothed);
			//C.r_End			();
			//break;
		case 5:
			C.r_Pass		("null",			"SunShaftsDisplay",	FALSE,	FALSE,	FALSE);
			C.r_Sampler_clf	("sScene",			r2_RT_generic0);
			C.r_Sampler_clf	("sSunShafts",		r2_RT_SunShaftsMaskSmoothed);//r2_RT_SunShaftsPass0);
			C.r_End			();
			break;
	}
}