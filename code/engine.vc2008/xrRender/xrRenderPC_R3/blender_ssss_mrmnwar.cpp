#include "stdafx.h"
#pragma hdrstop

#include "blender_ssss_mrmnwar.h"

CBlender_ssss_mrmnwar::CBlender_ssss_mrmnwar	()	{ description.CLS = 0; }
CBlender_ssss_mrmnwar::~CBlender_ssss_mrmnwar	()	{ }
 
void CBlender_ssss_mrmnwar::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);
	
	switch (C.iElement)
	{
		case 0:
			C.r_Pass		("stub_screen_space", "ssss_mrmnwar_mask", FALSE, FALSE, FALSE);
			C.r_dx10Texture	("s_position",		r2_RT_P);
			C.r_dx10Texture ("s_image",			r2_RT_generic0);

			C.r_dx10Sampler	("smp_nofilter");
			C.r_dx10Sampler	("smp_rtlinear");
			C.r_End			();
			break;
		case 1:
			C.r_Pass		("stub_screen_space", "ssss_mrmnwar_mask_blur", FALSE, FALSE, FALSE);
			C.r_dx10Texture ("s_mask",			r2_RT_SunShaftsMask);
			C.r_dx10Sampler	("smp_rtlinear");
			C.r_End			();
			break;
		case 2:
			C.r_Pass		("stub_screen_space", "ssss_mrmnwar_generation", FALSE, FALSE, FALSE);
			C.r_dx10Texture ("s_mask_blur",		r2_RT_SunShaftsMaskSmoothed);
			C.r_dx10Sampler	("smp_rtlinear");
			C.r_End			();
			break;
		case 3:
			C.r_Pass		("stub_screen_space", "ssss_mrmnwar_generation", FALSE, FALSE, FALSE);
			C.r_dx10Texture	("s_mask_blur",		r2_RT_SunShaftsPass0);
			C.r_dx10Sampler	("smp_rtlinear");
			C.r_End			();
			break;
		case 4:
			C.r_Pass		("stub_screen_space", "ssss_mrmnwar_display", FALSE, FALSE, FALSE);
			C.r_dx10Texture ("s_image",			r2_RT_generic0);
			C.r_dx10Texture ("s_sunshafts",		r2_RT_SunShaftsMaskSmoothed);
			C.r_dx10Sampler	("smp_rtlinear");
			C.r_End			();
			break;
	}
}
