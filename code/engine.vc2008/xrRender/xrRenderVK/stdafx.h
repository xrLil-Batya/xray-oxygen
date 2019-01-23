// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once

#pragma warning(disable:4995)
#include "../../xrEngine/stdafx.h"
#pragma warning(disable:4995 4005)
#include <vulkan\vulkan.h>
#include <d3dx9.h>
#include <ppl.h>
#pragma warning(default:4995 4005)
#pragma warning(disable:4714)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )
#pragma warning(disable:4237)

#include "Core/VKCommonTypes.h"

#include "../xrRender/Debug/dxPixEventWrapper.h"

#include "../xrRender/HW.h"
#include "../xrRender/Shader.h"
#include "../xrRender/R_Backend.h"
#include "../xrRender/R_Backend_Runtime.h"

#define		R_R1	1
#define		R_R2	2
#define		R_R3	3
#define		R_R4	4
#define		R_VK	5
#define		RENDER	R_VK

#include "../xrRender/resourcemanager.h"

#include "../../xrEngine/vis_common.h"
#include "../../xrEngine/render.h"
#include "../../xrEngine/_d3d_extensions.h"
#include "../../xrEngine/igame_level.h"
#include "../xrRender/blenders\blender.h"
#include "../xrRender/blenders\blender_clsid.h"
#include "../../xrParticles/psystem.h"
#include "../xrRender/xrRender_console.h"
#include "../xrRenderPC_R4/r4.h"

IC	void	jitter(CBlender_Compile& C)
{
//	C.r_dx10Texture("jitter0", JITTER(0));
//	C.r_dx10Texture("jitter1", JITTER(1));
//	C.r_dx10Texture("jitter2", JITTER(2));
//	C.r_dx10Texture("jitter3", JITTER(3));
//	C.r_dx10Texture("jitter4", JITTER(4));
//	C.r_dx10Texture("jitterMipped", r2_jitter_mipped);
//	C.r_dx10Sampler("smp_jitter");
}
