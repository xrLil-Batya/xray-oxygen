#include "stdafx.h"
#pragma hdrstop

#include "blender_fxaa.h"

CBlender_FXAA::CBlender_FXAA()	{ description.CLS = 0; }
CBlender_FXAA::~CBlender_FXAA()	{	}

void CBlender_FXAA::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
	case 0: // luminance pass
		C.r_Pass		("null", "fxaa_luma", FALSE, FALSE, FALSE);
		C.r_Sampler_clf	("s_image", r2_RT_generic0);
		C.r_End			();
		break;
    case 1: // main pass
        C.r_Pass		("null", "fxaa_main", FALSE, FALSE, FALSE);
        C.r_Sampler_clf	("s_image", r2_RT_generic0);
        C.r_End			();
        break;
    }
}