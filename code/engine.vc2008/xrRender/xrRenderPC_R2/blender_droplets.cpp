#include "stdafx.h"
#pragma hdrstop

#include "blender_droplets.h"

CBlender_droplets::CBlender_droplets	()	{	description.CLS		= 0;	}
CBlender_droplets::~CBlender_droplets	()	{	}

void CBlender_droplets::Compile(CBlender_Compile& C)
{
	IBlender::Compile	(C);
	C.r_Pass			("null",			"droplets",	FALSE,	FALSE,	FALSE);
	C.r_Sampler_clf		("s_image",			r2_RT_generic0);
	C.r_Sampler_clf		("s_droplets",		"shaders\\droplets");
	C.r_End				();
}